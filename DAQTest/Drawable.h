#pragma once
#include <SFML/Graphics.hpp>

class Drawable {
public:
	struct Rectangle {
		float x, y;
		float width, height;

		Rectangle(float x = 0, float y = 0, float width = 0, float height = 0);
	};

	Drawable(Rectangle rect);
	virtual ~Drawable();

	virtual void Draw(sf::RenderTarget& target) = 0;

	virtual void Tick();
	virtual void SetX(float num);
	virtual void SetY(float num);
	virtual void SetWidth(float num);
	virtual void SetHeight(float num);
	virtual void SetFocus(bool val);

	virtual Rectangle GetBounds() const;
	virtual bool HasFocus() const;

	virtual void SetMousePosition(float x, float y);
	virtual void Event(sf::Event& event);

	virtual bool MouseInBounds(float x, float y) const;

protected:
	Rectangle rect;
	float mouse_x, mouse_y;
	bool mouse_held;
	bool has_focus;
};