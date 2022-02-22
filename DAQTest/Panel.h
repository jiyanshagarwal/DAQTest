#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include "Drawable.h"
#include "ScrollBar.h"

class Panel : public Drawable {
public:
	Panel(Drawable::Rectangle rect);
	virtual ~Panel();

	void Draw(sf::RenderTarget& target) override;
	void Tick() override;
	void SetMousePosition(float x, float y) override;
	void Event(sf::Event& event) override;

	virtual void AddObject(Drawable* object, unsigned int layer);	//If two objects have the same layer, the user must guarentee they do not overlap.

	void XScroll(bool val);
	void YScroll(bool val);

	void SetX(float num) override;
	void SetY(float num) override;
	void SetWidth(float num) override;
	void SetHeight(float num) override;
	void SetFocus(bool val) override;

	virtual void SetColor(sf::Color color);
	virtual void SetBorderColor(sf::Color color);
	virtual void SetBorderWidth(float num);

	sf::Color GetColor() const;
	sf::Color GetBorderColor() const;
	float GetBorderWidth() const;

protected:
	std::multimap<unsigned int, Drawable*> objectsToDraw;
	Drawable* focused_object;

	float right_limit, bottom_limit;
	ScrollBar x_scrollbar, y_scrollbar;
	bool x_scrollable, y_scrollable;

	static const int SCROLLBAR_THICKNESS;

	sf::Color color;
	sf::Color border_color;
	float border_width;

	void calculateFocus();
	void calculateScroll();
	float XOffset() const;
	float YOffset() const;
};