#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

struct Point2D {
    float x;
    float y;
};

// B-Spline 구조체: 차수 n, de Boor 점, Knot Sequence
struct BSpline {
    int degree;
    std::vector<Point2D> controlPoints;
    std::vector<float> knots;
};

struct GLBuffer {
    unsigned int VAO;
    unsigned int VBO;
};

struct CurveObject {
    BSpline spline;
    std::vector<Point2D> sampledPoints;
    GLBuffer controlBuffer;
    GLBuffer curveBuffer;
};

GLBuffer createPointBuffer(const std::vector<Point2D>& points) {
    GLBuffer buffer;

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);

    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);

    glBufferData(
	GL_ARRAY_BUFFER,
	points.size() * sizeof(Point2D),
	points.data(),
	GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point2D), (void*)0);
    glEnableVertexAttribArray(0);

    return buffer;
}

// 커브 렌더
void drawCurve(const CurveObject& curve) {
    // B-spline
    glBindVertexArray(curve.curveBuffer.VAO);

    glDrawArrays(
	GL_LINE_STRIP,
	0,
	curve.sampledPoints.size()
    );

    // Control Polygon
    glBindVertexArray(curve.controlBuffer.VAO);

    glDrawArrays(
	GL_LINE_STRIP,
	0,
	curve.spline.controlPoints.size()
    );

    // Control Points
    glPointSize(10.0f);
    glDrawArrays(
	GL_POINTS,
	0,
	curve.spline.controlPoints.size()
    );
}

// 교점 렌더
void drawIntersections(
    const std::vector<Point2D> intersections,
    const GLBuffer buffer) 
{
    glBindVertexArray(buffer.VAO);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, intersections.size());
}

Point2D deBoor(const BSpline& curve, float u) {
    int n = curve.degree;
    int K = curve.knots.size();

    // u에 맞는 I 구하기
    // 정의역: [u_{n-1}, u_{K-n}]
    int I = -1;
    float uMin = curve.knots[n - 1];
    float uMax = curve.knots[K - n];

    // 양끝 I 처리
    if (u < uMin || u > uMax) {
	return { 0.0f, 0.0f };
    }

    if (u == uMax) {
	I = K - n - 1;
    }

    for (int i = n - 1; i < K - n; ++i) {
	if (curve.knots[i] == curve.knots[i + 1]) // 같으면 스킵
	    continue;
	if (curve.knots[i] <= u && u < curve.knots[i + 1]) {
	    I = i;
	    break;
	}
    }

    // 작업 배열 준비
    std::vector<Point2D> prev = curve.controlPoints;
    std::vector<Point2D> next = prev;

    // de Boor
    for (int k = 1; k <= n; ++k) {
	int start = I - n + k + 1;
	int end = I + 1;

	for (int i = start; i <= end; ++i) {
	    // a_i^k(u) = u - u_{i-1} / u_{i+n-k} - u_{i-1}
	    float alpha = (u - curve.knots[i - 1]) / (curve.knots[i + n - k] - curve.knots[i - 1]);

	    // 두 점 보간
	    next[i].x = (1.0f - alpha) * prev[i - 1].x + alpha * prev[i].x;
	    next[i].y = (1.0f - alpha) * prev[i - 1].y + alpha * prev[i].y;
	}
	prev = next;
    }

    return prev[I + 1]; // 최종 마지막 점
}

// B-Spline 샘플링
std::vector<Point2D> sampleBSpline(const BSpline& curve, int sampleCount) {
    std::vector<Point2D> points;

    int n = curve.degree;
    int K = curve.knots.size();

    float uMin = curve.knots[n - 1];
    float uMax = curve.knots[K - n];

    for (int i = 0; i <= sampleCount; ++i) {
	float t = static_cast<float>(i) / static_cast<float>(sampleCount);
	float u = uMin + t * (uMax - uMin); // uMin ~ uMax 값을 간격 t만큼
	points.push_back(deBoor(curve, u));
    }

    return points;
}

// 외적
float cross(Point2D p1, Point2D p2) {
    return (p1.x * p2.y) - (p1.y * p2.x);
}

int ccw(Point2D p1, Point2D p2, Point2D p3) {
    double ccw = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    
    if (ccw > 0) return 1; // 세 점 진행방향이 반시계 방향
    else if (ccw < 0) return -1; // 세 점 진행방향이 시계 방향
    else if (ccw == 0) return 0; // 세 점 진행방향이 일직선
}

// 선분 교차 검사
bool isSegmentIntersection(Point2D p1, Point2D p2, Point2D q1, Point2D q2, Point2D& intersection) {
    int ccw1 = ccw(p1, p2, q1) * ccw(p1, p2, q2);
    int ccw2 = ccw(q1, q2, p1) * ccw(q1, q2, p2);

    if (ccw1 <= 0 && ccw2 <= 0) { // 교점이 생기는 경우
	if (ccw1 == 0 && ccw2 == 0) { // 일직선으로 겹치는 경우
	    return std::fmin(p1.x, p2.x) <= std::fmax(q1.x, q2.x) &&
		std::fmin(q1.x, q2.x) <= std::fmax(p1.x, p2.x) &&
		std::fmin(p1.y, p2.y) <= std::fmax(q1.y, q2.y) &&
		std::fmin(q1.y, q2.y) <= std::fmax(p1.y, p2.y);
	}

	Point2D r = {
	    p2.x - p1.x,
	    p2.y - p1.y
	};

	Point2D s = {
	    q2.x - q1.x,
	    q2.y - q1.y
	};

	Point2D qp = {
	    q1.x - p1.x,
	    q1.y - p1.y
	};

	// 교점 계산
	float t = cross(qp, s) / cross(r, s);
	intersection.x = p1.x + t * r.x;
	intersection.y = p1.y + t * r.y;

	return true;
    }
    return false;
}

// Brute Force 방식으로 교점 찾기
std::vector<Point2D> findIntersections(
    const std::vector<Point2D>& a,
    const std::vector<Point2D>& b)
{
    std::vector<Point2D> result;

    for (int i = 0; i < a.size() - 1; ++i) {
	for (int j = 0; j < b.size() - 1; ++j) {
	    Point2D intersection;

	    if (isSegmentIntersection(
		a[i],
		a[i + 1],
		b[j],
		b[j + 1],
		intersection
	    )) {
		result.push_back(intersection);
	    }
	}
    }

    return result;
}

// VBO 갱신 함수
void updateBuffer(const GLBuffer& buffer, const std::vector<Point2D>& points) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(
	GL_ARRAY_BUFFER,
	points.size() * sizeof(Point2D),
	points.data(),
	GL_DYNAMIC_DRAW
    );
}

void updateSubBuffer(const GLBuffer& buffer, const std::vector<Point2D>& points) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferSubData(
	GL_ARRAY_BUFFER,
	0,
	points.size() * sizeof(Point2D),
	points.data()
    );
}

// 커브 모양 업데이트
void updateCurve(CurveObject& curve) {
    curve.sampledPoints = sampleBSpline(curve.spline, 100);
    updateBuffer(curve.controlBuffer, curve.spline.controlPoints);
    updateBuffer(curve.curveBuffer, curve.sampledPoints);
}
    

// -------------- AABB / BVH --------------

struct AABB {
    float minX;
    float minY;
    float maxX;
    float maxY;
};

AABB makeAABB(Point2D p1, Point2D p2) {
    AABB box;

    box.minX = std::min(p1.x, p2.x);
    box.maxX = std::max(p1.x, p2.x);

    box.minY = std::min(p1.y, p2.y);
    box.maxY = std::max(p1.y, p2.y);

    return box;
}

// AABB 합치기
AABB mergeAABB(const AABB& a, const AABB& b) {
    AABB result;

    result.minX = std::min(a.minX, b.minX);
    result.maxX = std::max(a.maxX, b.maxX);

    result.minY = std::min(a.minY, b.minY);
    result.maxY = std::max(a.maxY, b.maxY);

    return result;
}

bool isAABBOverlap(const AABB& a, const AABB& b) {
    // 왼쪽, 오른쪽, 위, 아래 하나라도 완전히 떨어져있면 안 겹침
    if (a.maxX < b.minX) return false;
    if (b.maxX < a.minX) return false;

    if (a.maxY < b.minY) return false;
    if (b.maxY < a.minY) return false;

    return true;
}

// 각 선분
struct Segment {
    Point2D p1;
    Point2D p2;
    AABB box;
};

std::vector<Segment> makeSegments(const std::vector<Point2D>& points) {
    std::vector<Segment> segments;

    for (int i = 0; i < points.size() - 1; i++) {
	Segment segment;

	segment.p1 = points[i];
	segment.p2 = points[i + 1];
	segment.box = makeAABB(segment.p1, segment.p2);

	segments.push_back(segment);
    }

    return segments;
}

// Segment 한 구간 감싸는 AABB 만들기
AABB computeRangeAABB(const std::vector<Segment> segments, int start, int end) {
    AABB box = segments[start].box;

    for (int i = start + 1; i < end; ++i) {
	box = mergeAABB(box, segments[i].box);
    }

    return box;
}

struct BVHNode {
    AABB box;

    // segment 범위
    int start;
    int end;

    BVHNode* left = nullptr;
    BVHNode* right = nullptr;
};

BVHNode* buildBVH(const std::vector<Segment>& segments, int start, int end) {
    BVHNode* node = new BVHNode;
    node->start = start;
    node->end = end;
    node->box = computeRangeAABB(segments, start, end);

    // Segment가 한 개면 leaf
    if (end - start == 1)
	return node;

    // 가운데에서 나누기
    int mid = (start + end) / 2;
    node->left = buildBVH(segments, start, mid);
    node->right = buildBVH(segments, mid, end);

    return node;
}

void findIntersectionsBVH(
    BVHNode* nodeA,
    BVHNode* nodeB,
    const std::vector<Segment>& segmentsA,
    const std::vector<Segment>& segmentsB,
    std::vector<Point2D>& result)
{
    // AABB 안 겹치면 바로 종료
    if (!isAABBOverlap(nodeA->box, nodeB->box)) {
	return;
    }

    bool leafA = (nodeA->end - nodeA->start == 1);
    bool leafB = (nodeB->end - nodeB->start == 1);

    // 둘 다 leaf면 실제 선분끼리 교차 검사
    if (leafA && leafB) {
	Segment segmentA = segmentsA[nodeA->start];
	Segment segmentB = segmentsB[nodeB->start];

	Point2D intersection;

	if (isSegmentIntersection(
	    segmentA.p1,
	    segmentA.p2,
	    segmentB.p1,
	    segmentB.p2,
	    intersection
	)) {
	    result.push_back(intersection);
	}

	return;
    }

    // leaf 아니면 더 내려감
    if (!leafA && !leafB) {
	findIntersectionsBVH(nodeA->left, nodeB->left, segmentsA, segmentsB, result);
	findIntersectionsBVH(nodeA->left, nodeB->right, segmentsA, segmentsB, result);
	findIntersectionsBVH(nodeA->right, nodeB->left, segmentsA, segmentsB, result);
	findIntersectionsBVH(nodeA->right, nodeB->right, segmentsA, segmentsB, result);
    }
    else if (!leafA && leafB) {
	findIntersectionsBVH(nodeA->left, nodeB, segmentsA, segmentsB, result);
	findIntersectionsBVH(nodeA->right, nodeB, segmentsA, segmentsB, result);
    }
    else if (leafA && !leafB) {
	findIntersectionsBVH(nodeA, nodeB->left, segmentsA, segmentsB, result);
	findIntersectionsBVH(nodeA, nodeB->right, segmentsA, segmentsB, result);
    } 
}

// -------------- AABB / BVH --------------

// Window 사이즈 변경 시 프레임 버퍼도 사이즈 맞게 변경
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "B-Spline Curve Intersection", NULL, NULL);
    if (window == NULL) {
	std::cout << "Failed to create GLFW window" << std::endl;
	glfwTerminate();
	return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	std::cout << "Failed to initialize GLAD" << std::endl;
	return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 커브 1
    BSpline spline1;
    spline1.degree = 3;
    spline1.controlPoints = {
	{-0.9f, -0.5f},
	{-0.7f,  0.4f},
	{-0.4f,  0.7f},
	{ 0.0f,  0.2f},
	{ 0.3f, -0.5f},
	{ 0.6f,  0.6f},
	{ 0.9f,  0.0f}
    };
    spline1.knots = {
	0.0f, 0.0f, 0.0f,
	1.0f, 2.0f, 3.0f,
	4.0f, 4.0f, 4.0f
    };

    // 커브 2
    BSpline spline2;
    spline2.degree = 3;
    spline2.controlPoints = {
	{-0.9f,  0.3f},
	{-0.6f, -0.6f},
	{-0.3f, -0.2f},
	{ 0.0f,  0.6f},
	{ 0.3f,  0.3f},
	{ 0.6f, -0.5f},
	{ 0.9f,  0.4f}
    };
    spline2.knots = {
	0.0f, 0.0f, 0.0f,
	1.0f, 2.0f, 3.0f,
	4.0f, 4.0f, 4.0f
    };

    // 커브
    CurveObject curve1;
    curve1.spline = spline1;
    curve1.sampledPoints = sampleBSpline(curve1.spline, 100);
    curve1.controlBuffer = createPointBuffer(curve1.spline.controlPoints);
    curve1.curveBuffer = createPointBuffer(curve1.sampledPoints);
    CurveObject curve2;
    curve2.spline = spline2;
    curve2.sampledPoints = sampleBSpline(curve2.spline, 100);
    curve2.controlBuffer = createPointBuffer(curve2.spline.controlPoints);
    curve2.curveBuffer = createPointBuffer(curve2.sampledPoints);

    std::vector<Segment> segments1;
    std::vector<Segment> segments2;

    BVHNode* root1 = nullptr;
    BVHNode* root2 = nullptr;

    // 교점
    std::vector<Point2D> intersections;
    GLBuffer intersectionBuffer = createPointBuffer(intersections);

    int selectedPoint = -1; // 선택한 점
    bool wasMousePressed = false;

    // render loop
    while (!glfwWindowShouldClose(window)) {
	// 창 크기 가져오기
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// 마우스 위치 가져오기
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	float x = 2.0f * mouseX / width - 1.0f;
	float y = 1.0f - 2.0f * mouseY / height;

	bool isMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	if (isMousePressed && !wasMousePressed) {
	    selectedPoint = -1;

	    // 마우스 근처 제어점 찾기
	    for (int i = 0; i < curve1.spline.controlPoints.size(); i++) {
		float dx = x - curve1.spline.controlPoints[i].x;
		float dy = y - curve1.spline.controlPoints[i].y;
		float distance = std::sqrt(dx * dx + dy * dy);

		if (distance < 0.05f) {
		    selectedPoint = i;
		    break;
		}
	    }
	}

	// Control Point 드래그
	if (isMousePressed && selectedPoint != -1) {
	    curve1.spline.controlPoints[selectedPoint] = { x, y };
	    updateCurve(curve1);
	}

	if (!isMousePressed) {
	    selectedPoint = -1;
	}

	wasMousePressed = isMousePressed;

	// Segment 만들기
	segments1 = makeSegments(curve1.sampledPoints);
	segments2 = makeSegments(curve2.sampledPoints);

	// BVH 트리 만들기
	root1 = buildBVH(segments1, 0, segments1.size());
	root2 = buildBVH(segments2, 0, segments2.size());

	// 교점 표시
	intersections.clear();
	//intersections = findIntersections(curve1.sampledPoints, curve2.sampledPoints);
	findIntersectionsBVH(root1, root2, segments1, segments2, intersections);

	glBindBuffer(GL_ARRAY_BUFFER, intersectionBuffer.VBO);
	glBufferData(
	    GL_ARRAY_BUFFER,
	    intersections.size() * sizeof(Point2D),
	    intersections.data(),
	    GL_DYNAMIC_DRAW
	);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	drawCurve(curve1);
	drawCurve(curve2);
	drawIntersections(intersections, intersectionBuffer);  

	glfwSwapBuffers(window);
	glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}