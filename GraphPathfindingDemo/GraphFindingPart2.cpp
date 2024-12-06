#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <GL/glut.h>

const int gridWidth = 6;
const int gridHeight = 3;
const int windowSize = 600;

std::unordered_map<char, std::vector<std::pair<char, int>>> graph;
std::unordered_map<char, std::pair<int, int>> positions; // Position on the grid
std::vector<char> path;

char startNode = 0;
char endNode = 0;

void createGraph() {
	int index = 0;
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			char node = 'a' + index;
			positions[node] = { x, y };
			std::vector<std::pair<char, int>> neighbors;

			for (int dy = -1; dy <= 1; ++dy) {
				for (int dx = -1; dx <= 1; ++dx) {
					if (dy == 0 && dx == 0) continue;
					int nx = x + dx, ny = y + dy;
					if (nx >= 0 && ny >= 0 && nx < gridWidth && ny < gridHeight) {
						char neighbor = 'a' + ny * gridWidth + nx;
						neighbors.emplace_back(neighbor, 1); // Default weight
					}
				}
			}
			graph[node] = neighbors;
			++index;
		}
	}

	// Assign 11 random heavy weights (5-10) to edges
	std::srand(std::time(0));
	for (int i = 0; i < 11; ++i) {
		char node = 'a' + (std::rand() % (gridWidth * gridHeight));
		if (!graph[node].empty()) {
			int neighborIndex = std::rand() % graph[node].size();
			graph[node][neighborIndex].second = 3 + (std::rand() % 8); // Weight between 3 and 10
		}
	}
}

std::vector<char> dijkstra(char start, char end) {
	std::unordered_map<char, int> dist;
	std::unordered_map<char, char> prev;
	for (auto& node : graph) dist[node.first] = INT_MAX;

	dist[start] = 0;
	std::priority_queue<std::pair<int, char>, std::vector<std::pair<int, char>>, std::greater<>> pq;
	pq.push({ 0, start });

	while (!pq.empty()) {
		char current = pq.top().second;
		pq.pop();

		if (current == end) break;

		for (auto& neighbor : graph[current]) {
			char next = neighbor.first;
			int weight = neighbor.second;
			int newDist = dist[current] + weight;
			if (newDist < dist[next]) {
				dist[next] = newDist;
				prev[next] = current;
				pq.push({ newDist, next });
			}
		}
	}

	std::vector<char> shortestPath;
	for (char at = end; at != 0; at = prev[at]) shortestPath.push_back(at);
	std::reverse(shortestPath.begin(), shortestPath.end());
	return shortestPath;
}

void drawGrid() {
	glColor3f(0.8f, 0.8f, 0.8f);
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x + 1, y);
			glVertex2f(x + 1, y + 1);
			glVertex2f(x, y + 1);
			glEnd();
		}
	}
}

void drawEdges() {
	for (const auto& [node, neighbors] : graph) {
		auto [x1, y1] = positions[node];
		for (const auto& [neighbor, weight] : neighbors) {
			auto [x2, y2] = positions[neighbor];
			if (weight > 1) glColor3f(1.0f, 0.0f, 0.0f); // Red for heavy weights
			else glColor3f(0.5f, 0.5f, 0.5f); // Light gray for default weight

			glLineWidth(weight > 1 ? 2.0f : 1.0f); // Thicker line for heavy weights
			glBegin(GL_LINES);
			glVertex2f(x1 + 0.5, y1 + 0.5);
			glVertex2f(x2 + 0.5, y2 + 0.5);
			glEnd();
		}
	}
}

void drawPath(const std::vector<char>& path) {
	if (path.empty()) return;

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	for (size_t i = 0; i < path.size() - 1; ++i) {
		auto [x1, y1] = positions[path[i]];
		auto [x2, y2] = positions[path[i + 1]];
		glVertex2f(x1 + 0.5, y1 + 0.5);
		glVertex2f(x2 + 0.5, y2 + 0.5);
	}
	glEnd();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	drawGrid();
	drawEdges();
	drawPath(path);
	glFlush();
}

void getUserInput() {
	std::cout << "Grid Layout:\n";
	int index = 0;
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			std::cout << static_cast<char>('a' + index) << " ";
			++index;
		}
		std::cout << "\n";
	}
	std::cout << "\nEnter start node: ";
	std::cin >> startNode;
	std::cout << "Enter end node: ";
	std::cin >> endNode;

	if (graph.find(startNode) == graph.end() || graph.find(endNode) == graph.end()) {
		std::cout << "Invalid nodes. Exiting.\n";
		exit(1);
	}
	path = dijkstra(startNode, endNode);
}

int main(int argc, char** argv) {
	createGraph();
	getUserInput();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(windowSize, windowSize);
	glutCreateWindow("Dijkstra Pathfinding Visualization (Weighted)");
	glOrtho(0, gridWidth, gridHeight, 0, -1, 1);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
