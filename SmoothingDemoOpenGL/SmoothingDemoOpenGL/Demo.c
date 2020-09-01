#include "Demo.h"

HWND hMainWindow;
HDC hdc;
HGLRC hrc;
RECT rect;
int clock;
float angle;
float speed = 0.18f;

float fov = 60.0f;
float zNear = 0.1f;
float zFar = 1000.0f;

int verticesCount;
Vertex *vertices;
Color *colors;

Color backColor =
{
	0.0f, 0.0f, 0.0f
};

Color c =
{
	0.5f, 0.5f, 0.5f
};

WNDCLASSEX wndClass =
{
	sizeof(WNDCLASSEX), 0, WindowProc, 0, 0, 0, 0, 0, 0, 0, CLASS_NAME, 0
};

PIXELFORMATDESCRIPTOR pfd =
{
	sizeof(PIXELFORMATDESCRIPTOR),
		1, PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW,
		PFD_TYPE_RGBA, COLOR_DEPTH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		COLOR_DEPTH, 0, 0, PFD_MAIN_PLANE, 0, PFD_MAIN_PLANE, 0, 0
};

void main()
{
	MSG msg;

	RegisterClassEx(&wndClass);
	hMainWindow = CreateWindowEx(0, CLASS_NAME, 0, WS_CUSTOM, 0, 0, 0, 0, 0, 0, 0, 0);
	
	Init();

	while (GetMessage(&msg, 0, 0, 0))
	{
		DispatchMessage(&msg);
	}
	
	ExitProcess(0);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
		{
			Subdivide();
			return 0;
		}
		if (wParam != VK_ESCAPE)
		{
			break;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		Draw();
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Init()
{
	GetClientRect(hMainWindow, &rect);
	ShowCursor(false);

	clock = GetTickCount();

	hdc = GetDC(hMainWindow);
	SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);

	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);
	glViewport(0, 0, rect.right, rect.bottom);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)rect.right / rect.bottom, zNear, zFar);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GenerateMesh(&cube);
}

void GenerateMesh(Mesh *mesh)
{
	int i, j;
	int index;
	int vertexIndex;
	Triangle *t;

	index = 0;
	verticesCount = mesh->trianglesCount * 3;
	vertices = (Vertex *)calloc(verticesCount, sizeof(Vertex));
	colors = (Color *)calloc(verticesCount, sizeof(Vertex));

	for (i = 0; i < mesh->trianglesCount; i++)
	{
		t = &mesh->indices[i];
		for (j = 0; j < 3; j++)
		{
			vertexIndex = t->vertices[j];

			vertices[index] = mesh->vertices[vertexIndex];
			colors[index] = c;

			index++;
		}
	}
}

void Draw()
{
	int time;

	time = GetTickCount();
	if (time - clock > 10)
	{
		clock = time;
		angle += speed * PI;
	}

	glClearColor(backColor.r, backColor.g, backColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, SIZE * 1.8f, SIZE * 3.6f, 0, 0, 0, 0, 1.0f, 0);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glColorPointer(3, GL_FLOAT, 0, colors);
	glDrawArrays(GL_TRIANGLES, 0, verticesCount);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	SwapBuffers(hdc);
}

void Subdivide()
{
	int i, j;
	int index;
	int trianglesCount;
	int newVerticesCount;
	Vertex *newVertices;
	Color *newColors;
	Vertex center;
	Vertex *v;
	Vertex oldV[3];
	Vertex middlePoints[3];
	Vertex newV[12];

	newVerticesCount = verticesCount * 4;
	trianglesCount = verticesCount / 3;
	index = 0;

	newVertices = (Vertex *)calloc(newVerticesCount, sizeof(Vertex));
	newColors = (Color *)calloc(newVerticesCount, sizeof(Color));
	
	center.x = 0;
	center.y = 0;
	center.z = 0;
	for (i = 0; i < verticesCount; i++)
	{
		v = &vertices[i];
		center.x += v->x;
		center.y += v->y;
		center.z += v->z;
	}
	center.x /= verticesCount;
	center.y /= verticesCount;
	center.z /= verticesCount;

	for (i = 0; i < trianglesCount; i++)
	{
		for (j = 0; j < 3; j++)
		{
			oldV[j] = vertices[i * 3 + j];
		}

		GetSmoothPoint(&oldV[0], &oldV[1], &center, &middlePoints[0]);
		GetSmoothPoint(&oldV[1], &oldV[2], &center, &middlePoints[1]);
		GetSmoothPoint(&oldV[0], &oldV[2], &center, &middlePoints[2]);

		newV[0] = oldV[0];
		newV[1] = middlePoints[0];
		newV[2] = middlePoints[2];

		newV[3] = middlePoints[0];
		newV[4] = oldV[1];
		newV[5] = middlePoints[1];

		newV[6] = middlePoints[0];
		newV[7] = middlePoints[1];
		newV[8] = middlePoints[2];

		newV[9] = middlePoints[2];
		newV[10] = middlePoints[1];
		newV[11] = oldV[2];
		 
		for (j = 0; j < 12; j++)
		{
			newVertices[index] = newV[j];
			newColors[index] = c;
			index++;
		}
	}

	free(vertices);
	free(colors);

	verticesCount = newVerticesCount;
	vertices = newVertices;
	colors = newColors;
}

void GetSmoothPoint(Vertex *v1, Vertex *v2, Vertex *center, Vertex *result)
{
	Vertex middlePoint;
	float length1, length2, averageLength;

	middlePoint.x = (v1->x + v2->x) / 2;
	middlePoint.y = (v1->y + v2->y) / 2;
	middlePoint.z = (v1->z + v2->z) / 2;

	result->x = middlePoint.x - center->x;
	result->y = middlePoint.y - center->y;
	result->z = middlePoint.z - center->z;

	length1 = GetVerticesDistance(center, v1);
	length2 = GetVerticesDistance(center, v2);
	averageLength = (length1 + length2) / 2;

	Normalize(result);
	result->x *= averageLength;
	result->y *= averageLength;
	result->z *= averageLength;
}

float GetVerticesDistance(Vertex *v1, Vertex *v2)
{
	return (float)sqrt((v1->x - v2->x) * (v1->x - v2->x)
		+ (v1->y - v2->y) * (v1->y - v2->y)
		+ (v1->z - v2->z) * (v1->z - v2->z));
}

void Normalize(Vertex *v)
{
	float length;

	length = (float)sqrt((v->x) * (v->x ) + (v->y) * (v->y) + (v->z) * (v->z));
	
	v->x /= length;
	v->y /= length;
	v->z /= length;
}
