#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "Types.h"
#include "Cube.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define SIZE 100
#define PI 3.14f

#define CLASS_NAME "Smoothing test"
#define WS_CUSTOM WS_VISIBLE | WS_POPUP | WS_MAXIMIZE

void main();
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Init();
GenerateMesh(Mesh *mesh);
void Draw();
void Subdivide();
void GetSmoothPoint(Vertex *v1, Vertex *v2, Vertex *center, Vertex *result);
float GetVerticesDistance(Vertex *v1, Vertex *v2);
void Normalize(Vertex *v);