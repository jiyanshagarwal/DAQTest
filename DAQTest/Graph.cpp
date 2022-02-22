#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Graph.h"

Graph::Point::Point(float x, float y) {
	this->x = x;
	this->y = y;
}

Graph::Graph(Drawable::Rectangle rect) : Drawable(rect) {
	scale_x = 25;
	scale_y = 25;
	origin_x = rect.x + rect.width / 2;
	origin_y = rect.y + rect.height / 2;

	background_color = sf::Color::White;
	line_color = sf::Color::Black;
	lighter_line_color = sf::Color(200, 200, 200);
	border_color = sf::Color::Red;

	prev_mouse_x = 0;
	prev_mouse_y = 0;
	this->ComputeGridLines();
}

void Graph::Tick() {
	if (mouse_held && has_focus) {
		origin_x += mouse_x - prev_mouse_x;
		origin_y += mouse_y - prev_mouse_y;
		ComputeGridLines();
		ComputeDataVertexArray();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && this->has_focus) {
		SetXScale(GetXScale() * 1.05f);
		SetYScale(GetYScale() * 1.05f);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && this->has_focus) {
		SetXScale(GetXScale() / 1.05f);
		SetYScale(GetYScale() / 1.05f);
	}
}

void Graph::Draw(sf::RenderTarget& target) {
	sf::RectangleShape border(sf::Vector2f(rect.width, rect.height));
	border.setPosition(sf::Vector2f(rect.x, rect.y));
	border.setFillColor(background_color);
	border.setOutlineThickness(5);
	border.setOutlineColor(border_color);
	target.draw(border);

	if (grid_lines.size() > 0) {
		target.draw(&grid_lines[0], grid_lines.size(), sf::Lines);
	}

	if (data_to_draw.size() > 0) {
		target.draw(&data_to_draw[0], data_to_draw.size(), sf::Points);
	}
}

void Graph::SetMousePosition(float x, float y) {
	prev_mouse_x = mouse_x;
	prev_mouse_y = mouse_y;

	mouse_x = x;
	mouse_y = y;
}

void Graph::SetData(std::vector<Point>& data) {
	this->data = data;
	ComputeDataVertexArray();
}

void Graph::AddData(Point val) {
	data.push_back(val);
	ComputeDataVertexArray();
}

void Graph::SetFunctions(std::unordered_map<float(*)(float), sf::Color> functions) {
	this->functions = functions;
	ComputeDataVertexArray();
}

void Graph::AddFunction(float(*func)(float), sf::Color color) {
	functions.emplace(func, color);
	ComputeDataVertexArray();
}

void Graph::SetColor(sf::Color color) {
	background_color = color;

	//Gets the opposite color from the background to provide constrast
	line_color = sf::Color(255 - background_color.r, 255 - background_color.g, 255 - background_color.b);

	float tint_factor = 0.6f;
	lighter_line_color = sf::Color(line_color.r * tint_factor, line_color.g * tint_factor, line_color.b * tint_factor) + background_color;
	lighter_line_color = sf::Color(255 - lighter_line_color.r, 255 - lighter_line_color.g, 255 - lighter_line_color.b);

	ComputeGridLines();
}

void Graph::SetBorderColor(sf::Color color) {
	border_color = color;
}

void Graph::SetXScale(float scale) {
	scale_x = scale >= 0 ? scale : scale_x;
	ComputeGridLines();
	ComputeDataVertexArray();
}

void Graph::SetYScale(float scale) {
	scale_y = scale >= 0 ? scale : scale_y;
	ComputeGridLines();
	ComputeDataVertexArray();
}

float Graph::GetXScale() const {
	return scale_x;
}

float Graph::GetYScale() const {
	return scale_y;
}

void Graph::SetX(float num) {
	Drawable::SetX(num);
	ComputeGridLines();
	ComputeDataVertexArray();
}

void Graph::SetY(float num) {
	Drawable::SetY(num);
	ComputeGridLines();
	ComputeDataVertexArray();
}

void Graph::SetWidth(float num) {
	Drawable::SetWidth(num);
	ComputeGridLines();
	ComputeDataVertexArray();
}

void Graph::SetHeight(float num) {
	Drawable::SetHeight(num);
	ComputeGridLines();
	ComputeDataVertexArray();
}

sf::Color Graph::GetColor() const {
	return background_color;
}

sf::Color Graph::GetBorderColor() const {
	return border_color;
}

void Graph::ComputeGridLines() {
	grid_lines.clear();

	//----------------------------------[Compute Grid Lines]----------------------------------//
	double range = (rect.height / 2) / scale_x;
	double tick_magnitude = std::pow(10.0, std::floor(std::log10(range)));

	if (range / tick_magnitude >= 5) {}
	else if (range / (tick_magnitude / 2.0) >= 5) {
		tick_magnitude /= 2.0;
	}
	else {
		tick_magnitude /= 5.0;
	}
	tick_magnitude *= scale_x;

	double pos = origin_x - ((int)((origin_x - rect.x) / tick_magnitude) * tick_magnitude);
	pos = MouseInBounds(pos, rect.y) ? pos : pos += tick_magnitude;	//This is necessary due to rounding errors when the origin_x is less than x.

	while (MouseInBounds(pos, rect.y)) {
		grid_lines.push_back(sf::Vertex(sf::Vector2f(pos, rect.y), lighter_line_color));
		grid_lines.push_back(sf::Vertex(sf::Vector2f(pos, rect.y + rect.height), lighter_line_color));
		pos += tick_magnitude;
	}

	//------------------------------------------------------------------------------------------------------

	range = (rect.height / 2) / scale_y;
	tick_magnitude = std::pow(10.0, std::floor(std::log10(range)));

	if (range / tick_magnitude >= 5) {}
	else if (range / (tick_magnitude / 2.0) >= 5) {
		tick_magnitude /= 2.0;
	}
	else {
		tick_magnitude /= 5.0;
	}
	tick_magnitude *= scale_y;

	pos = origin_y - ((int)((origin_y - rect.y) / tick_magnitude) * tick_magnitude);
	pos = MouseInBounds(rect.x, pos) ? pos : pos += tick_magnitude;	//This is necessary due to rounding errors when the origin_y is less than y.

	while (MouseInBounds(rect.x, pos)) {
		grid_lines.push_back(sf::Vertex(sf::Vector2f(rect.x, pos), lighter_line_color));
		grid_lines.push_back(sf::Vertex(sf::Vector2f(rect.x + rect.width, pos), lighter_line_color));
		pos += tick_magnitude;
	}

	//----------------------------------[Compute Axes]----------------------------------//

	//X-axis
	if (MouseInBounds(rect.x, origin_y)) {
		grid_lines.push_back(sf::Vertex(sf::Vector2f(rect.x, origin_y), line_color));
		grid_lines.push_back(sf::Vertex(sf::Vector2f(rect.x + rect.width, origin_y), line_color));
	}

	//Y-axis
	if (MouseInBounds(origin_x, rect.y)) {
		grid_lines.push_back(sf::Vertex(sf::Vector2f(origin_x, rect.y), line_color));
		grid_lines.push_back(sf::Vertex(sf::Vector2f(origin_x, rect.y + rect.height), line_color));
	}
}

void Graph::ComputeDataVertexArray() {
	data_to_draw.clear();

	for (unsigned int i = 0; i < data.size(); i++) {
		Point p = GraphToScreenCoordinates(data[i].x, data[i].y);

		if (MouseInBounds(p.x, p.y)) {
			//Create a 3x3 pixel square that represents the point to make it more visible.
			for (float row = p.x - 1; row <= p.x + 1; row++) {
				for (float col = p.y - 1; col <= p.y + 1; col++) {
					data_to_draw.push_back(sf::Vertex(sf::Vector2f(row, col), sf::Color::Red));
				}
			}
		}
	}

	/* In order to draw functions smoothly, the following code dynamically changes the delta_x parameter based on the derivative of the 
	function. When the slope is steep, delta_x is reduced. Conversely, at shallow slopes, delta_x is increased.*/

	for (auto iter = functions.begin(); iter != functions.end(); iter++) {
		//Converts a point x to graph space, gets its correspondig y value, and then converts that y value back into screen space.
		auto ScreenX_to_ScreenY = [this, iter](float x) { return GraphToScreenCoordinates(0, iter->first(ScreenToGraphCoordinates(x - 1, 0).x)).y; };

		Point prev_point = Point(rect.x - 1, ScreenX_to_ScreenY(rect.x - 1));
		float delta_x = 1;

		for (Point point = Point(rect.x, prev_point.y); point.x <= rect.x + rect.width; point.x += delta_x) {
			prev_point.y = point.y;
			point.y = ScreenX_to_ScreenY(point.x);

			if (MouseInBounds(point.x, point.y)) {
				data_to_draw.push_back(sf::Vertex(sf::Vector2f(point.x, point.y), iter->second));
			}

			if (prev_point.y - point.y != 0) {
				delta_x = std::abs((prev_point.x - point.x) / (prev_point.y - point.y));	// Delta x is one over instantaneous slope (1/m).
			}
			else {
				delta_x = 1;
			}

			// Delta x must be clamped for balance of speed and drawing niceness.
			delta_x = std::max(0.01f, std::min(delta_x, 1.0f));

			prev_point.x = point.x;
		}
	}
}

Graph::Point Graph::ScreenToGraphCoordinates(float x, float y) {
	x = (x - origin_x) / scale_x;
	y = (origin_y - y) / scale_y;

	return Point(x, y);
}

Graph::Point Graph::GraphToScreenCoordinates(float x, float y) {
	x = (x * scale_x) + origin_x;
	y = origin_y - (y * scale_y);

	return Point(x, y);
}