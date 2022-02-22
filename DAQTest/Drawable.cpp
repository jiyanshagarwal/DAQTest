#include "Drawable.h"

Drawable::Rectangle::Rectangle(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

Drawable::Drawable(Rectangle rect) {
	this->rect = rect;
	this->mouse_x = 0;
	this->mouse_y = 0;
	this->mouse_held = false;
	this->has_focus = false;
}

Drawable::~Drawable() {}

void Drawable::Tick() {}

void Drawable::SetX(float num) {
	rect.x = num;
}

void Drawable::SetY(float num) {
	rect.y = num;
}

void Drawable::SetWidth(float num) {
	rect.width = num;
}

void Drawable::SetHeight(float num) {
	rect.height = num;
}

void Drawable::SetFocus(bool val) {
	has_focus = val;
}

Drawable::Rectangle Drawable::GetBounds() const {
	return rect;
}

bool Drawable::HasFocus() const {
	return has_focus;
}

void Drawable::SetMousePosition(float x, float y) {
	mouse_x = x;
	mouse_y = y;
}

void Drawable::Event(sf::Event& event) {
	switch (event.type) {
	case sf::Event::MouseButtonPressed:
		if (event.mouseButton.button == sf::Mouse::Left && MouseInBounds(mouse_x, mouse_y)) {
			mouse_held = true;
		}
		break;
	case sf::Event::MouseButtonReleased:
		if (event.mouseButton.button == sf::Mouse::Left) {
			mouse_held = false;
		}
		break;
	}
}

bool Drawable::MouseInBounds(float x, float y) const {
	return (x >= rect.x && (x < rect.x + rect.width) && y >= rect.y && (y < rect.y + rect.height));
}