#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <math.h>
#include "sadi_labview.h"
#include "Drawable.h"
#include "Panel.h"
#include "Graph.h"
using std::cout;

const std::map<int, std::string> read_vals = { {0, "SADI_RESET (FT_OK)"}, {1, "FT_INVLAID_HANDLE"}, {2, "FT_DEVICE_NOT_FOUND"}, {3, "FT_DEVICE_NOT_OPENED"},
											 {4, "FT_IO_ERROR"}, {5, "FT_INSUFFICIENT_RESOURCES"}, {18, "FT_OTHER_ERROR"}, {17, "FT_NOT_SUPPORTED"},
											 {32768, "DATA_RECEIVED"}, {32769, "NO_DATA_AVAILABLE"}, {32770, "NO_SADI"} };

const std::map<int, std::string> states = { {0, "Boot"}, {1, "Setup"}, {2, "Start Initiated"}, {3, "Running"} };

const std::map<std::string, int> gaines = { {"OFF", 0}, {"GAIN_1X (10.24VPP)", 1}, {"GAIN_2X (5.12VPP)", 2}, {"GAIN_4X (2.56VPP)", 3},
									  {"GAIN_8X (1.28VPP)", 4}, {"GAIN_16X (0.64VPP)", 5} };

bool reset() {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::milliseconds;

	auto start_time = high_resolution_clock::now();
	while (getSadiReady() == 0 && duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count() < 5) {
		sadiReset();
	}

	std::string read_val = read_vals.at(sadiRead());
	return getSadiReady() == 1 and (read_val == "DATA_RECEIVED" or read_val == "NO_DATA_AVAILABLE");
}

bool initialize(int gain_list[], bool enable_list[]) {
	cout << "State: " << states.at(getState()) << "\n";
	cout << "Set Frequency: " << sendSetTimerFreq(20000) << "\n";

	for (int i = 0; i < 4; i++) { sendAdcInit(std::pow(2, i), gain_list[i]); }

	cout << "State: " << states.at(getState()) << "\n\n";
	cout << "Read Value: " << read_vals.at(sadiRead()) << "\n";

	bool valid = false;

	for (int i = 0; i < 4; i++) {
		enable_list[i] = getAdcInfo_enable(i);
		cout << "ADC" << i << " Enabled: " << enable_list[i] << "\n";
		valid = valid || !enable_list[i];
	}

	return valid;
}

bool start() {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::milliseconds;


	sadiStart();
	cout << "State: " << states.at(getState()) << "\n";

	auto start_time = high_resolution_clock::now();
	while (states.at(getState()) != "Running" && duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count() < 5) {
		sadiRead();
	}

	return states.at(getState()) == "Running";
}

void readData(std::vector<double> data[], int gain_list[]) {
	int data_packet[4] = { 0, 0, 0, 0 };
	sadiRead();
	//cout << "Read Value: " << read_vals.at(sadiRead()) << "\n";
	bool has_data = getDataLatest(data_packet);
	//cout << "New Data: " << has_data << "\n";

	if (has_data) {
		for (int i = 0; i < 4; i++) {
			double voltage = (10.24 / 8191.0) * getLowWord(data_packet[i]) / std::pow(2, (gain_list[i] - 1));
			data[i].push_back(voltage);
		}
	}
}

void eventLoop(sf::RenderWindow window, Panel windowPanel) {
	sf::Color focused_color = sf::Color(0, 76, 135);
	sf::Color unfocused_color = sf::Color(100, 100, 100);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			windowPanel.Event(event);
		}

		if (window.hasFocus()) {
			windowPanel.SetBorderColor(focused_color);
		}
		else {
			windowPanel.SetBorderColor(unfocused_color);
		}

		window.clear();

		windowPanel.SetMousePosition((float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y);
		windowPanel.Tick();
		windowPanel.Draw(window);

		window.display();
	}
}

int main() {
#pragma region Window Initilization
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(1200, 800), "Lab 1", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	Panel windowPanel(Drawable::Rectangle(0, 0, window.getSize().x, window.getSize().y));
	windowPanel.SetFocus(true);
	windowPanel.SetColor(sf::Color(70, 70, 70));
	windowPanel.SetBorderWidth(-1);
#pragma endregion

#pragma region Graph
	Graph graph(Drawable::Rectangle(10, 20, windowPanel.GetBounds().width - 20, windowPanel.GetBounds().height - 40));
	graph.SetColor(sf::Color(50, 50, 50));
	graph.SetBorderColor(sf::Color::Black);
	graph.AddData(Graph::Point(1, 1));
#pragma endregion

	if (connectSadi() == 0) {
		if (sadiReset()) {
			std::vector<double> data[4] = { std::vector<double>(), std::vector<double>(), std::vector<double>(), std::vector<double>() };
			int gain_list[4] = { gaines["GAIN_2X (5.12VPP)"], gaines["GAIN_2X (5.12VPP)"], gaines["GAIN_2X (5.12VPP)"], gaines["GAIN_2X (5.12VPP)"] };
			bool enable_list[4] = { 1, 1, 1, 1 };

			bool initialized = initialize(gain_list, enable_list);

			if (initialized) {
				if (sadiStart()) {
					readData(data, gain_list);
				}
				else {
					cout << "Failed to start.\n";
				}
			}
			else {
				cout << "Failed to initialize.\n";
			}
		}
		else {
			cout << "Failed to reset.\n";
		}
	}
	else {
		cout << "Failed to connect.\n";
	}

	sadiStop();
	cout << "Sadi Disconnected: " << (disconnectSadi() == 0 ? "Yes" : "No") << std::endl;
}