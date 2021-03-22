#pragma once
#include <QtWidgets>

#define EPSILON 0.000000001;

struct Edge
{
	QPoint startPoint;
	QPoint endPoint;
	int deltaY;
	double x;
	double w;

	bool operator>(const Edge& edge) // courtesy of Alex Filip
	{
		return startPoint.y() > edge.startPoint.y();
	}
};

enum Interpolation
{
	NearestNeighbor = 0, Barycentric1 = 1, Barycentric2 = 2
};

enum CurveType
{
	HermitCurve = 0, BezierCurve = 1, CoonsCurve = 2
};

class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QString name = "";
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QRgb* data = nullptr;
	QPainter* painter = nullptr;

	QColor defaultColor0 = QColor("#ED1C24");
	QColor defaultColor1 = QColor("#00AD33");
	QColor defaultColor2 = QColor("#1F75FE");

	// pomocne funkcie
	void swapPoints(QPoint& point1, QPoint& point2); // prehodenie 2 bodov
	void printEdges(QVector<Edge> polygonEdges); // vypisat hrany polygonu
	void printPoints(QVector<QPoint> polygonPoints);

	void bubbleSortEdgesY(QVector<Edge>& polygonEdges); // usporiadanie hran podla y
	void bubbleSortEdgesX(QVector<Edge>& polygonEdges); // usporiadnanie hran podla x
	void bubbleSortTrianglePoints(QVector<QPoint>& trianglePoints); // usporiadanie bodov trojuholnika
	void setEdgesOfPolygon(QVector<QPoint> polygonPoints, QVector<Edge>& polygonEdges); // vytvorenie hran pre polygon

	// Hermitovske kubicke polynomy
	double F0(double t) { return (2.0 * t * t * t - 3.0 * t * t + 1.0); }
	double F1(double t) { return (-2.0 * t * t * t + 3.0 * t * t); }
	double F2(double t) { return (t * t * t - 2.0 * t * t + t); }
	double F3(double t) { return (t * t * t - t * t); }

	// Kubicke polynomy
	double B0(double t) { return (-(1.0 / 6.0) * t * t * t + 0.5 * t * t - 0.5 * t + (1.0 / 6.0)); }
	double B1(double t) { return (0.5 * t * t * t - t * t + (2.0 / 3.0)); }
	double B2(double t) { return (-0.5 * t * t * t + 0.5 * t * t + 0.5 * t + (1.0 / 6.0)); }
	double B3(double t) { return ((1.0 / 6.0) * t * t * t); }

	// vypocet farby pixela pre trojuholnik
	QColor getNearestNeighborColor(QVector<QPoint> trianglePoints, QPoint currentPoint);
	QColor getBarycentricColor(QVector<QPoint> T, QPoint P);
	QColor getBarycentricDistanceColor(QVector<QPoint> T, QPoint P);

	// kreslenie, orezavanie, vyfarbovanie
	void drawBresenhamChosenX(QPoint point1, QPoint point2, QColor color);
	void drawBresenhamChosenY(QPoint point1, QPoint point2, QColor color);
	void drawGeometry(QVector<QPoint> geometryPoints, QColor penColor, QColor fillColor, int lineAlgorithm, int interpolationMethod);
	void trimLine(QVector<QPoint> currentLine, QColor color, int lineAlgorithm);
	void trimPolygon(QVector<QPoint> V, QColor penColor, QColor fillColor, int lineAlgorithm, int interpolationMethod);
	void fillPolygonScanLineAlgorithm(QVector<QPoint> polygonPoints, QColor fillColor);
	void fillTriangleScanLine(QVector<QPoint> T, int interpolationMethod);

public:
	ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	// funkcie na kreslenie
	void drawLineDDA(QPoint point1, QPoint point2, QColor color);
	void drawLineBresenham(QPoint point1, QPoint point2, QColor color);
	void createLineWithAlgorithm(QPoint point1, QPoint point2, QColor color, int lineAlgorithm, bool shouldDrawPoints = false);
	void drawCircumference(QPoint point1, QPoint point2, QColor color);
	void drawPoint(QPoint point, QColor color);
	void drawPoints(QVector<QPoint> points, QColor color);

	void createGeometry(QVector<QPoint>& geometryPoints, QColor color, QColor fillColor, int lineAlgorithm, int interpolationMethod);
	void createCurve(QVector<QPoint>& curvePoints, QColor color, int curveType);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();

	//Data functions
	QRgb* getData() { return data; }
	void setPixel(int x, int y, const QColor& color);
	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	bool isInside(int x, int y) { return (x >= 0 && y >= 0 && x < img->width() && y < img->height()) ? true : false; }

	//Get/Set functions
	QString getName() { return name; }
	void setName(QString newName) { name = newName; }

	void setPainter() { painter = new QPainter(img); }
	void setDataPtr() { data = reinterpret_cast<QRgb*>(img->bits()); }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	void clear(QColor color = Qt::white);

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};