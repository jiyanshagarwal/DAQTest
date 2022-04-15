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

int count = 0;

bool reset() {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::milliseconds;

	auto start_time = high_resolution_clock::now();
	while (SADI_LabVIEW::getSadiReady() == 0 && duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count() < 5000) {
		SADI_LabVIEW::sadiReset();
	}

	std::string read_val = read_vals.at(SADI_LabVIEW::sadiRead());
	return SADI_LabVIEW::getSadiReady() == 1 and (read_val == "DATA_RECEIVED" or read_val == "NO_DATA_AVAILABLE");
}

bool initialize(int gain_list[], bool enable_list[]) {
	cout << "State: " << states.at(SADI_LabVIEW::getState()) << "\n";
	cout << "Set Frequency: " << SADI_LabVIEW::sendSetTimerFreq(20000) << "\n";

	for (int i = 0; i < 4; i++) { SADI_LabVIEW::sendAdcInit(std::pow(2, i), gain_list[i]); }

	cout << "State: " << states.at(SADI_LabVIEW::getState()) << "\n\n";
	cout << "Read Value: " << read_vals.at(SADI_LabVIEW::sadiRead()) << "\n";

	bool valid = false;

	for (int i = 0; i < 4; i++) {
		enable_list[i] = SADI_LabVIEW::getAdcInfo_enable(i);
		cout << "ADC" << i << " Enabled: " << enable_list[i] << "\n";
		valid = valid || !enable_list[i];
	}

	return valid;
}

bool start() {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::milliseconds;

	SADI_LabVIEW::sadiStart();
	cout << "State: " << states.at(SADI_LabVIEW::getState()) << "\n";

	auto start_time = high_resolution_clock::now();
	while (states.at(SADI_LabVIEW::getState()) != "Running" && duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count() < 5000) {
		SADI_LabVIEW::sadiRead();
	}

	return states.at(SADI_LabVIEW::getState()) == "Running";
}

void readData(std::vector<double>* data, int gain_list[], Graph& graph) {
	int data_packet[7] = { 0, 0, 0, 0 };
	SADI_LabVIEW::sadiRead();
	//cout << "Read Value: " << read_vals.at(sadiRead()) << "\n";
	bool has_data = SADI_LabVIEW::getDataLatest(data_packet);
	//cout << "New Data: " << has_data << "\n";

	if (has_data) {
		for (int i = 0; i < 4; i++) {
			double voltage = (10.24 / 8191.0) * SADI_LabVIEW::getLowWord(data_packet[i]) / std::pow(2, (gain_list[i] - 1));
			data[i].push_back(voltage);
		}

		float graph_division = graph.GetBounds().width / 1000;
		if (count > 1000) {
			graph.origin_x -= graph_division;
		}

		count++;

		graph.AddData(Graph::Point(data[0].size(), data[0].back()));

		for (int i = 0; i < 4; i++) { cout << data[i].back() << " "; }
		cout << "\n";
	}
}

void eventLoop(sf::RenderWindow& window, Panel& windowPanel, Graph& graph, int gain_list[]) {
	sf::Color focused_color = sf::Color(0, 76, 135);
	sf::Color unfocused_color = sf::Color(100, 100, 100);

	std::vector<double> data[4] = { std::vector<double>(), std::vector<double>(), std::vector<double>(), std::vector<double>() };

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			
			windowPanel.Event(event);
		}

		if (window.hasFocus()) {
			windowPanel.SetBorderColor(focused_color);
		}
		else {
			windowPanel.SetBorderColor(unfocused_color);
		}

		readData(data, gain_list, graph);

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
	//graph.SetColor(sf::Color(50, 50, 50));
	graph.SetBorderColor(sf::Color::Black);
	graph.SetXScale(graph.GetBounds().width / 1000);
	graph.SetYScale(0.8 * graph.GetBounds().height / 3.5);
	graph.origin_x = graph.GetBounds().x;
	graph.origin_y = graph.GetBounds().y + graph.GetBounds().height;
	windowPanel.AddObject(&graph, 1);
#pragma endregion

	if (SADI_LabVIEW::connectSadi() == 0) {
		if (reset()) {
			int gain_list[4] = { gaines.at("GAIN_2X (5.12VPP)"), gaines.at("GAIN_2X (5.12VPP)"), gaines.at("GAIN_2X (5.12VPP)"), gaines.at("GAIN_2X (5.12VPP)")};
			bool enable_list[4] = { 1, 1, 1, 1 };

			bool initialized = initialize(gain_list, enable_list);

			if (initialized) {
				if (start()) {
					eventLoop(window, windowPanel, graph, gain_list);
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

	SADI_LabVIEW::sadiStop();
	cout << "Sadi Disconnected: " << (SADI_LabVIEW::disconnectSadi() == 0 ? "Yes" : "No") << std::endl;
}