#pragma once
#include <SFML/Graphics.hpp>
#include "Drawable.h"
#include "Button.h"

class ScrollBar : public Drawable {
public:
	enum class Orientation {
		VERTICAL,
		HORIZONTAL
	};

	ScrollBar(float x = 0, float y = 0, float thickness = 0, float length = 0, Orientation orientation = Orientation::VERTICAL);
	ScrollBar& operator=(const ScrollBar& ref);
	void Tick() override;
	void Draw(sf::RenderTarget& target) override;
	void Event(sf::Event& event) override;
	void SetMousePosition(float x, float y) override;

	void ResetGripPosition();

	void SetBarSizeRatio(float val);
	void SetLength(float val);
	void SetThickness(float val);

	void SetX(float num) override;
	void SetY(float num) override;
	void SetWidth(float num) override;
	void SetHeight(float num) override;
	void SetFocus(bool val);

	void SetGripColor(sf::Color color);
	void SetTrackColor(sf::Color color);

	float GetGripPosition() const;
	sf::Color GetGripColor() const;
	sf::Color GetTrackColor() const;
	bool isActive() const;

private:
	//Base Properties (provided by the user)
	float length, thickness;
	Orientation orientation;
	float grip_position;
	float bar_size_ratio;

	float prev_mouse_x, prev_mouse_y;

	//Derived Properties (useful internally)
	float trackLength;
	float gripLength;
	float gripThickness;
	float gripOffset;

	static const float MIN_GRIP_SIZE;

	sf::Color grip_color;
	sf::Color track_color;

	Button scroll_increase_button;
	Button scroll_decrease_button;

	void CalculateScroll(int key);
	bool MouseWithinGrip() const;
	void InitializeButtons();
};