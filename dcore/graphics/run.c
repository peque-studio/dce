#include <GLFW/glfw3.h>
#include <dcore/graphics.h>
#include <dcore/graphics/internal.h>

bool dcgShouldClose(DCgState *state) { return glfwWindowShouldClose(state->window); }

void dcgClose(DCgState *state) { glfwSetWindowShouldClose(state->window, GLFW_TRUE); }

void dcgGetMousePosition(DCgState *state, DCmVector2i mousePosition) {
	double xpos, ypos;
	glfwGetCursorPos(state->window, &xpos, &ypos);
	mousePosition[0] = xpos;
	mousePosition[1] = ypos;
}

void dcgUpdate(DCgState *state) { glfwPollEvents(); }
