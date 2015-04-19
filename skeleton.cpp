#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"


using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};
Intersection closestIntersection;
// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
	);

void PrintTriangle(Triangle triangle);


int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

void Draw()
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);
	//load model
	LoadTestModel(triangles);

	vec3 dir,color;
	float f = SCREEN_HEIGHT / 2;
	vec3 cameraPos(0,0,-2);
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			vec3 dir(x - SCREEN_HEIGHT / 2, y - SCREEN_WIDTH/2, f);
	
			if (ClosestIntersection(cameraPos, dir, triangles, closestIntersection))
			{
				color = triangles[closestIntersection.triangleIndex].color;
					PutPixelSDL(screen, x, y, color);
			}

		}
	}

	

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection)
{
	vec3 e1,e2,b,result,e;
	float distance_temp;
	bool flag=false;
	closestIntersection.distance = std::numeric_limits<float>::max();
	for(int i=0;i<triangles.size();++i)
	{
		e1 = triangles[i].v1-triangles[i].v0;
		e2 = triangles[i].v2-triangles[i].v0;
		b = start - triangles[i].v0;
		mat3 A(-dir, e1, e2);

		e = glm::inverse(A)*b;
		result = e.y*e1 + e.z*e2+triangles[i].v0;
		
		if (e.y>=0 && e.z >= 0 && (e.y+e.z <= 1))
		{
			//the distance might be negative, no consideration here
			flag = true;
			distance_temp = glm::length(result-start);
			if (distance_temp < closestIntersection.distance){
				closestIntersection.distance = distance_temp;
				closestIntersection.position = e.y*e1+e.z*e2;
				closestIntersection.triangleIndex = i;
			}
		}
	}
	return flag;
}
