#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;


struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;

vector<Triangle> triangles;
float focalLength = 750;   //(Ingen aning om hur det här påverkar, prövade runt )
vec3 cameraPos(0, 0, -4);  //(samma här)

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();

/*
Kollar om det finns triangel som intersectar med vektor. om inte false annars true o sätter intersectgrejen. 
startar på start, går i dir(ection), tringles är alla trianglar (? vafn) och closestinstersect är närmaste intersect mellan ljusstråle o ytan sett från kameran
*/
bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
);

int main(int argc, char* argv[])
{
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.
	LoadTestModel(triangles);

	while (NoQuitMessageSDL())
	{
		Update();
		Draw();
	}

	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

void Draw()
{
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	vec3 color(0, 0, 0); //svart, Kanske borde ligga som global? 
	for (int y = 0; y<SCREEN_HEIGHT; ++y) //b
	{
		for (int x = 0; x<SCREEN_WIDTH; ++x) //b
		{
			vec3 dir(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2, focalLength); //d = (x-W/2, y-H/2, f) (24)
			Intersection inter;
			if (ClosestIntersection(cameraPos, dir, triangles, inter)) {
				PutPixelSDL(screen, x, y, triangles[inter.triangleIndex].color);
			}
			else {
				PutPixelSDL(screen, x, y, color); //sätt tillbaka till color
			}
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
) {

	
	closestIntersection.distance = std::numeric_limits<float>::max(); //sätt grejen till max eller nått
	bool foundIntersection = false;

	// 
	for (size_t i = 0; i < triangles.size(); ++i) {

		Triangle triangle = triangles[i];
		// Calculate the value of x, (t, u, v)
		
		vec3 v0 = triangle.v0;
		vec3 v1 = triangle.v1;
		vec3 v2 = triangle.v2;
		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = start - v0;
		mat3 A(-dir, e1, e2);
		vec3 x = glm::inverse(A) * b;

		
		//Kör in i triangel
		float t = x.x;
		float u = x.y;
		float v = x.z;

		// Om det blir en intersection
		if (0 <= u && 0 <= v && u + v <= 1 && t >= 0) {
			// Om intersection är närmare än nuvarande
			if (t < closestIntersection.distance) {
				closestIntersection.distance = t;
				closestIntersection.position = x;
				closestIntersection.triangleIndex = i;
			}
			foundIntersection = true;
		}
	}
	return foundIntersection;
}