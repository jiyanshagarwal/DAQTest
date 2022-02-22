#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "Drawable.h"

class Graph : public Drawable {
public:
	struct Point {
		float x, y;
		Point(float x = 0, float y = 0);
	};

	Graph(Drawable::Rectangle rect = Drawable::Rectangle(0, 0, 100, 100));
	void Tick() override;
	void Draw(sf::RenderTarget& target) override;

	void SetMousePosition(float x, float y) override;

	void SetData(std::vector<Point>& data);
	void AddData(Point val);
	void SetFunctions(std::unordered_map<float(*)(float), sf::Color> functions);
	void AddFunction(float(*func)(float), sf::Color color);

	void SetColor(sf::Color color);
	void SetBorderColor(sf::Color color);

	void SetXScale(float scale);
	void SetYScale(float scale);
	void SetX(float num) override;
	void SetY(float num) override;
	void SetWidth(float num) override;
	void SetHeight(float num) override;

	float GetXScale() const;
	float GetYScale() const;
	sf::Color GetColor() const;
	sf::Color GetBorderColor() const;

private:
	double scale_x, scale_y;
	float origin_x, origin_y;
	float prev_mouse_x, prev_mouse_y;

	sf::Color background_color;
	sf::Color line_color;
	sf::Color lighter_line_color;
	sf::Color border_color;

	std::vector<Point> data;
	std::unordered_map<float(*)(float), sf::Color> functions;

	std::vector<sf::Vertex> grid_lines;
	std::vector<sf::Vertex> data_to_draw;

	void ComputeGridLines();
	void ComputeDataVertexArray();
	Point ScreenToGraphCoordinates(float x, float y);
	Point GraphToScreenCoordinates(float x, float y);
};