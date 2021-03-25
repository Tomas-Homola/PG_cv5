#include   "ViewerWidget.h"

void ViewerWidget::swapPoints(QPoint& point1, QPoint& point2)
{
	int temp = point1.x();
	point1.setX(point2.x());
	point2.setX(temp);
	
	temp = point1.y();
	point1.setY(point2.y());
	point2.setY(temp);
}

void ViewerWidget::printEdges(QVector<Edge> polygonEdges)
{
	qDebug() << "printing edges:";
	for (int i = 0; i < polygonEdges.size(); i++)
		qDebug() << "polygonEdges[" << i << "]:" << polygonEdges[i].startPoint << polygonEdges[i].endPoint << "deltaY:" << polygonEdges[i].deltaY << "x:" <<  polygonEdges[i].x << "w:" << polygonEdges[i].w;
}
void ViewerWidget::printPoints(QVector<QPoint> polygonPoints)
{
	qDebug() << "printing points:";
	for (int i = 0; i < polygonPoints.size(); i++)
		qDebug() << "point[" << i << "]: (" << polygonPoints[i].x() << ", " << polygonPoints[i].y() << ")";
}
void ViewerWidget::bubbleSortEdgesY(QVector<Edge>& polygonEdges)
{
	int i = 0, k = 0, n = polygonEdges.size();
	Edge tempEdge;

	// opisane z internetu
	for (k = 0; k < n - 1; k++)
	{
		for (i = 0; i < (n - k - 1); i++)
		{
			if (polygonEdges[i] > polygonEdges[i + 1])
			{
				tempEdge = polygonEdges[i];
				polygonEdges[i] = polygonEdges[i + 1];
				polygonEdges[i + 1] = tempEdge;
			}
		}

	}
}
void ViewerWidget::bubbleSortEdgesX(QVector<Edge>& polygonEdges)
{
	int n = polygonEdges.size();
	Edge tempEdge;

	for (int k = 0; k < n - 1; k++)
	{
		for (int i = 0; i < (n - k - 1); i++)
		{
			if (polygonEdges[i].x > polygonEdges[i + 1].x)
			{
				tempEdge = polygonEdges[i];
				polygonEdges[i] = polygonEdges[i + 1];
				polygonEdges[i + 1] = tempEdge;
			}
		}

	}
}
void ViewerWidget::bubbleSortTrianglePoints(QVector<QPoint>& trianglePoints)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3 - i - 1; j++)
		{
			if (trianglePoints[j].y() > trianglePoints[j + 1].y())
				swapPoints(trianglePoints[j], trianglePoints[j + 1]);
		}
	}

	if (trianglePoints[0].y() == trianglePoints[1].y())
	{
		if (trianglePoints[0].x() > trianglePoints[1].x())
			swapPoints(trianglePoints[0], trianglePoints[1]);
	}
	else if (trianglePoints[1].y() == trianglePoints[2].y())
	{
		if (trianglePoints[1].x() > trianglePoints[2].x())
			swapPoints(trianglePoints[1], trianglePoints[2]);
	}
}
void ViewerWidget::setEdgesOfPolygon(QVector<QPoint> polygonPoints, QVector<Edge>& polygonEdges)
{
	polygonEdges.clear();
	int size = polygonPoints.size();
	int deltaY = 0, deltaX = 0;
	double slope = 0.0;
	Edge newEdge;

	for (int i = 0; i < size; i++)
	{
		//https://stackoverflow.com/questions/61613618/how-can-i-iterate-over-the-vertices-of-a-polygon-and-compare-two-vertices-with-e
		if (polygonPoints[(i + 1) % size].y() > polygonPoints[i].y())
		{
			newEdge.startPoint = polygonPoints[i];
			newEdge.endPoint = polygonPoints[(i + 1) % size];
		}
		else
		{
			newEdge.startPoint = polygonPoints[(i + 1) % size];
			newEdge.endPoint = polygonPoints[i];
		}

		deltaX = newEdge.endPoint.x() - newEdge.startPoint.x();
		deltaY = newEdge.endPoint.y() - newEdge.startPoint.y();

		if (deltaY != 0)
		{
			if (deltaX == 0)
				newEdge.w = 0.0;
			else
			{
				slope = (double)deltaY / (double)deltaX;
				newEdge.w = 1.0 / slope;
			}

			newEdge.endPoint.setY(newEdge.endPoint.y() - 1);
			newEdge.deltaY = newEdge.endPoint.y() - newEdge.startPoint.y();
			newEdge.x = (double)newEdge.startPoint.x();

			polygonEdges.push_back(newEdge);
		}
	}

	//printEdges(polygonEdges);
	bubbleSortEdgesY(polygonEdges);
	//printEdges(polygonEdges);
}

// vypocet farby pixela
QColor ViewerWidget::getNearestNeighborColor(QVector<QPoint> trianglePoints, QPoint currentPoint)
{
	QColor defaultColor("#000000");
	int d0 = 0, d1 = 0, d2 = 0;

	d0 = std::sqrt((trianglePoints[0].x() - currentPoint.x()) * (trianglePoints[0].x() - currentPoint.x()) + (trianglePoints[0].y() - currentPoint.y()) * (trianglePoints[0].y() - currentPoint.y()));
	d1 = std::sqrt((trianglePoints[1].x() - currentPoint.x()) * (trianglePoints[1].x() - currentPoint.x()) + (trianglePoints[1].y() - currentPoint.y()) * (trianglePoints[1].y() - currentPoint.y()));
	d2 = std::sqrt((trianglePoints[2].x() - currentPoint.x()) * (trianglePoints[2].x() - currentPoint.x()) + (trianglePoints[2].y() - currentPoint.y()) * (trianglePoints[2].y() - currentPoint.y()));

	if (d0 <= d1 && d0 <= d2)
		return defaultColor0;
	else if (d1 <= d0 && d1 <= d2)
		return defaultColor1;
	else if (d2 <= d0 && d2 <= d1)
		return defaultColor2;
	else
		return defaultColor;
}
QColor ViewerWidget::getBarycentricColor(QVector<QPoint> T, QPoint P)
{
	// tu uz sa mi nechcelo pisat dlhe nazvy premennych
	QColor outputColor("#000000");
	int red = 0, green = 0, blue = 0;
	double w1 = 0.0, w2 = 0.0, w3 = 0.0;
	double lambda0 = 0.0, lambda1 = 0.0, lambda2 = 0.0;

	/*w1 = (((double)T[1].y() - (double)T[2].y()) * ((double)P.x() - (double)T[2].x()) + ((double)T[2].x() - (double)T[1].x()) * ((double)P.y() - (double)T[2].y())) / (((double)T[1].y() - (double)T[2].y()) * ((double)T[0].x() - (double)T[2].x()) + ((double)T[2].x() - (double)T[1].x()) * ((double)T[0].y() - (double)T[2].y()));
	w2 = (((double)T[2].y() - (double)T[0].y()) * ((double)P.x() - (double)T[2].x()) + ((double)T[0].x() - (double)T[2].x()) * ((double)P.y() - (double)T[2].y())) / (((double)T[1].y() - (double)T[2].y()) * ((double)T[0].x() - (double)T[2].x()) + ((double)T[2].x() - (double)T[1].x()) * ((double)T[0].y() - (double)T[2].y()));
	w3 = 1.0 - w1 - w2;
	
	red = static_cast<int>(w1 * defaultColor0.red() + w2 * defaultColor1.red() + w3 * defaultColor2.red());
	green = static_cast<int>(w1 * defaultColor0.green() + w2 * defaultColor1.green() + w3 * defaultColor2.green());
	blue = static_cast<int>(w1 * defaultColor0.blue() + w2 * defaultColor1.blue() + w3 * defaultColor2.blue());*/

	lambda0 = qAbs(((double)T[1].x() - (double)P.x())*((double)T[2].y() - (double)P.y()) - ((double)T[1].y() - (double)P.y())*((double)T[2].x() - (double)P.x())) / qAbs(((double)T[1].x() - (double)T[0].x()) * ((double)T[2].y() - (double)T[0].y()) - ((double)T[1].y() - (double)T[0].y()) * ((double)T[2].x() - T[0].x()));
	
	lambda1 = qAbs(((double)T[0].x() - (double)P.x()) * ((double)T[2].y() - (double)P.y()) - ((double)T[0].y() - (double)P.y()) * ((double)T[2].x() - (double)P.x())) / qAbs(((double)T[1].x() - (double)T[0].x()) * ((double)T[2].y() - (double)T[0].y()) - ((double)T[1].y() - (double)T[0].y()) * ((double)T[2].x() - T[0].x()));

	lambda2 = 1.0 - lambda0 - lambda1;

	red = static_cast<int>(lambda0 * defaultColor0.red() + lambda1 * defaultColor1.red() + lambda2 * defaultColor2.red());
	green = static_cast<int>(lambda0 * defaultColor0.green() + lambda1 * defaultColor1.green() + lambda2 * defaultColor2.green());
	blue = static_cast<int>(lambda0 * defaultColor0.blue() + lambda1 * defaultColor1.blue() + lambda2 * defaultColor2.blue());

	outputColor.setRed(red);
	outputColor.setGreen(green);
	outputColor.setBlue(blue);

	return outputColor;
}
QColor ViewerWidget::getBarycentricDistanceColor(QVector<QPoint> T, QPoint P)
{
	// tu uz sa mi nechcelo pisat dlhe nazvy premennych
	QColor outputColor("#000000");
	int red = 0, green = 0, blue = 0;
	double d1 = 0.0, d2 = 0.0, d3 = 0.0, w1 = 0.0, w2 = 0.0, w3 = 0.0;

	// trochu iny sposob tejto interpolacie: https://codeplea.com/triangular-interpolation
	d1 = std::sqrt((T[0].x() - P.x()) * (T[0].x() - P.x()) + (T[0].y() - P.y()) * (T[0].y() - P.y()));
	d2 = std::sqrt((T[1].x() - P.x()) * (T[1].x() - P.x()) + (T[1].y() - P.y()) * (T[1].y() - P.y()));
	d3 = std::sqrt((T[2].x() - P.x()) * (T[2].x() - P.x()) + (T[2].y() - P.y()) * (T[2].y() - P.y()));

	w1 = 1.0 / d1;
	w2 = 1.0 / d2;
	w3 = 1.0 / d3;

	red = static_cast<int>((w1 * defaultColor0.red() + w2 * defaultColor1.red() + w3 * defaultColor2.red()) / (w1 + w2 + w3));
	green = static_cast<int>((w1 * defaultColor0.green() + w2 * defaultColor1.green() + w3 * defaultColor2.green()) / (w1 + w2 + w3));
	blue = static_cast<int>((w1 * defaultColor0.blue() + w2 * defaultColor1.blue() + w3 * defaultColor2.blue()) / (w1 + w2 + w3));

	outputColor.setRed(red);
	outputColor.setGreen(green);
	outputColor.setBlue(blue);

	return outputColor;
}

void ViewerWidget::drawBresenhamChosenX(QPoint point1, QPoint point2, QColor color)
{
	if (point1.x() > point2.x()) // ak sa klikol prvy bod viac vpravo
		swapPoints(point1, point2);

	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();
	double slope = static_cast<double>(deltaY) / static_cast<double>(deltaX);
	int k1 = 0;
	int k2 = 0;
	int p = 0;
	int x = 0;
	int y = 0;

	if (slope >= 0 && slope < 1)
	{
		k1 = 2 * deltaY; k2 = 2 * (deltaY - deltaX);
		p = 2 * deltaY - deltaX;

		x = point1.x(); y = point1.y();

		setPixel(x, y, color);

		while (x < point2.x())
		{
			x++;
			if (p > 0)
			{
				y++;
				p += k2;
			}
			else
				p += k1;

			setPixel(x, y, color);
		}
	}
	else if (slope > -1 && slope <= 0)
	{
		k1 = 2 * deltaY; k2 = 2 * (deltaY + deltaX);
		p = 2 * deltaY + deltaX;

		x = point1.x(); y = point1.y();

		setPixel(x, y, color);

		while (x < point2.x())
		{
			x++;
			if (p < 0)
			{
				y--;
				p += k2;
			}
			else
				p += k1;

			setPixel(x, y, color);
		}
	}
}
void ViewerWidget::drawBresenhamChosenY(QPoint point1, QPoint point2, QColor color)
{
	if (point1.y() > point2.y()) // ak sa klikol prvy bod nizsie ako druhy bod
		swapPoints(point1, point2);

	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();

	double slope = static_cast<double>(deltaY) / static_cast<double>(deltaX);
	int k1 = 0;
	int k2 = 0;
	int p = 0;
	int x = 0;
	int y = 0;

	if (slope >= 1)
	{
		k1 = 2 * deltaX; k2 = 2 * (deltaX - deltaY);
		p = 2 * deltaX - deltaY;

		x = point1.x(); y = point1.y();

		setPixel(x, y, color);

		while (y < point2.y())
		{
			y++;

			if (p > 0)
			{
				x++;
				p += k2;
			}
			else
				p += k1;

			setPixel(x, y, color);
		}
	}
	else if (slope <= -1)
	{
		k1 = 2 * deltaX; k2 = 2 * (deltaX + deltaY);
		p = 2 * deltaX + deltaY;

		x = point1.x(); y = point1.y();

		setPixel(x, y, color);

		while (y < point2.y())
		{
			y++;

			if (p < 0)
			{
				x--;
				p += k2;
			}
			else
				p += k1;

			setPixel(x, y, color);
		}
	}
}
void ViewerWidget::drawGeometry(QVector<QPoint> geometryPoints, QColor penColor, QColor fillColor, int lineAlgorithm, int interpolationMethod)
{
	if (geometryPoints.size() == 2) // usecka
		createLineWithAlgorithm(geometryPoints.at(0), geometryPoints.at(1), penColor, lineAlgorithm);
	else if (geometryPoints.size() > 2) // polygon alebo trojuholnik
	{
		// vyfarbenie
		if (geometryPoints.size() == 3)
			fillTriangleScanLine(geometryPoints, interpolationMethod);
		else
			fillPolygonScanLineAlgorithm(geometryPoints, fillColor);

		// hranice polygonu
		for (int i = 1; i <= geometryPoints.size(); i++)
		{
			if (i == geometryPoints.size())
				createLineWithAlgorithm(geometryPoints.at(0), geometryPoints.at(i - 1), penColor, lineAlgorithm);
			else
				createLineWithAlgorithm(geometryPoints.at(i), geometryPoints.at(i - 1), penColor, lineAlgorithm);
		}
	}
}
void ViewerWidget::trimLine(QVector<QPoint> currentLine, QColor color, int lineAlgorithm)
{
	int imgHeight = getImgHeight();
	int imgWidth = getImgWidth();
	QPoint E[4]; // pole vrcholov obrazka
	E[0] = QPoint(0, 0); E[3] = QPoint(imgWidth, 0);
	E[1] = QPoint(0, imgHeight); E[2] = QPoint(imgWidth, imgHeight);
	QPoint P1 = currentLine[0], P2 = currentLine[1];
	QPoint newP1(0, 0), newP2(0, 0);
	QVector<QPoint> newLine;
	QPoint vectorD = P2 - P1, vectorW(0, 0), vectorE(0, 0), normalE(0, 0);
	double tL = 0.0, tU = 1.0, t = 0.0;
	int dotProductDN = 0, dotProductWN = 0;

	bool areInside = false;
	bool shouldTrim = false;

	if ((P1.x() >= 0 && P1.x() <= imgWidth && P1.y() >= 0 && P1.y() <= imgHeight) || (P2.x() >= 0 && P2.x() <= imgWidth && P2.y() >= 0 && P2.y() <= imgHeight))
		areInside = true;

	//https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
	if (!areInside) // ci sa usecka nachadza v nejakom rohu tak, ze oba body su uz mimo, ale mala by sa este osekavat
	{
		int intersections = 0;
		double s = 0.0, t = 0.0;
		int denom = 0;
		int upperS = 0;
		int upperT = 0;

		for (int i = 0; i < 4; i++)
		{
			if (i == 3)
				vectorE = E[0] - E[3];
			else
				vectorE = E[i + 1] - E[i];

			denom = -vectorE.x() * vectorD.y() + vectorD.x() * vectorE.y();
			upperS = -vectorD.y() * (P1.x() - E[i].x()) + vectorD.x() * (P1.y() - E[i].y());
			upperT = vectorE.x() * (P1.y() - E[i].y()) - vectorE.y() * (P1.x() - E[i].x());
			s = static_cast<double>(upperS) / denom;
			t = static_cast<double>(upperT) / denom;

			if (s >= 0.0 && s <= 1.0 && t >= 0 && t <= 1.0)
				intersections++;
		}

		if (intersections != 2)
			shouldTrim = false;
		else
			shouldTrim = true;
	}

	if (areInside || shouldTrim)
	{
		qDebug() << "trimming line";

		for (int i = 0; i < 4; i++)
		{
			// direct vector hrany obrazka
			if (i == 3)
				vectorE = E[0] - E[3];
			else
				vectorE = E[i + 1] - E[i];

			// z direct vectora spravime normalu: (x, y) -> (y, -x)
			normalE.setX(vectorE.y()); normalE.setY(-vectorE.x());
			vectorW = P1 - E[i];

			// skalarne suciny
			dotProductDN = QPoint::dotProduct(vectorD, normalE);
			dotProductWN = QPoint::dotProduct(vectorW, normalE);

			if (dotProductDN != 0)
			{
				t = static_cast<double>(-dotProductWN) / dotProductDN;

				if (dotProductDN > 0 && t <= 1.0)
					tL = std::max(t, tL);
				else if (dotProductDN < 0 && t >= 0.0)
					tU = std::min(t, tU);
			}
		}
		qDebug() << "tL:" << tL << "\ttU:" << tU;
		if (tL == 0.0 && tU == 1.0)
			drawGeometry(currentLine, color, Qt::white, lineAlgorithm, 0);
		else if (tL < tU)
		{
			newP1.setX(static_cast<int>(P1.x() + ((double)P2.x() - P1.x()) * tL));
			newP1.setY(static_cast<int>(P1.y() + ((double)P2.y() - P1.y()) * tL));

			newP2.setX(static_cast<int>(P1.x() + ((double)P2.x() - P1.x()) * tU));
			newP2.setY(static_cast<int>(P1.y() + ((double)P2.y() - P1.y()) * tU));

			newLine.push_back(newP1); newLine.push_back(newP2);

			drawGeometry(newLine, color, Qt::white, lineAlgorithm, 0);
		}
	}
	else
		drawGeometry(currentLine, color, Qt::white, lineAlgorithm, 0);
}
void ViewerWidget::trimPolygon(QVector<QPoint> V, QColor penColor, QColor fillColor, int lineAlgorithm, int interpolationMethod)
{
	//QVector<QPoint> V = polygonPoints; // kopia bodov
	QVector<QPoint> W;
	QPoint S(0, 0);
	int xMin[4] = { 0,0, -getImgWidth() + 1, -getImgHeight() + 1 }; // poznamka pre autora: obrazok v poznamkach ku tymto hodnotam; z nejakeho dovodu ak tam nie je +1, to nekresli na spodnu hranu obrazka
	int temp = 0;

	for (int i = 0; i < 4; i++)
	{
		if (V.size() != 0)
			S = V[V.size() - 1];

		for (int j = 0; j < V.size(); j++)
		{
			if (V.at(j).x() >= xMin[i])
			{
				if (S.x() >= xMin[i])
					W.push_back(V[j]);
				else
				{
					temp = static_cast<int>(S.y() + ((double)xMin[i] - S.x()) * ((double)V[j].y() - S.y()) / ((double)V[j].x() - S.x()) + 0.5);
					W.push_back(QPoint(xMin[i], temp)); // priesecnik P
					W.push_back(V[j]);
				}
			}
			else
			{
				if (S.x() >= xMin[i])
				{
					temp = static_cast<int>(S.y() + ((double)xMin[i] - S.x()) * ((double)V[j].y() - S.y()) / ((double)V[j].x() - S.x()) + 0.5);
					W.push_back(QPoint(xMin[i], temp)); // priesecnik P
				}
			}

			S = V.at(j);
		}

		V.clear();

		for (int k = 0; k < W.size(); k++)
			V.push_back(QPoint(W[k].y(), -W[k].x()));

		W.clear();
	}
	drawGeometry(V, penColor, fillColor, lineAlgorithm, interpolationMethod);
}
void ViewerWidget::fillPolygonScanLineAlgorithm(QVector<QPoint> polygonPoints, QColor fillColor)
{
	QVector<Edge> polygonEdges;
	QVector<QVector<Edge>> TH;
	QVector<Edge> ZAH;
	QVector<int> deleteZAH;
	int yMin = 0, yMax = 0, y = 0;
	int xStart = 0, xEnd = 0;
	int index = 0;

	setEdgesOfPolygon(polygonPoints, polygonEdges); // vytvorenie hran polygonu

	yMin = polygonEdges[0].startPoint.y(); // minimalna hdonota y

	for (int i = 0; i < polygonEdges.size(); i++) // yMax si treba zistit takto, nie tak, ako je to v pdf, ze to je z poslednej hrany y-hodnota endPointu -> nie vzdy to plati
		if (polygonEdges[i].endPoint.y() > yMax)
			yMax = polygonEdges[i].endPoint.y();

	y = yMin;

	TH.resize(yMax - yMin + 1); // ked tam nebolo trochu vacsie, padalo to pri orezavani

	for (int i = 0; i < polygonEdges.size(); i++) // priprava TH
	{
		index = polygonEdges[i].startPoint.y() - yMin;
		TH[index].push_back(polygonEdges[i]);
	}

	for (int i = 0; i < TH.size(); i++)
	{
		if (!TH[i].isEmpty())
			for (int j = 0; j < TH[i].size(); j++) // priradenie hran z TH do ZAH
				ZAH.append(TH[i][j]);

		bubbleSortEdgesX(ZAH); // usporiadanie ZAH podla x

		for (int j = 0; j < ZAH.size(); j++) // po dvojiciach prechadzanie cez ZAH
		{
			if (j % 2 == 0)
			{
				if (static_cast<int>(ZAH[j].x) != static_cast<int>(ZAH[j + 1].x))
				{
					xStart = static_cast<int>(ZAH[j].x);
					xEnd = static_cast<int>(ZAH[j + 1].x);

					if (xEnd - xStart != 0)
						for (int k = 1; k <= (xEnd - xStart); k++)
							setPixel(static_cast<int>(ZAH[j].x) + k, y, fillColor);
				}
			}
			// ci treba vymazat danu hranu zo ZAH
			if (ZAH[j].deltaY == 0)
				deleteZAH.push_back(j);

			// aktualizovanie hdonot
			ZAH[j].deltaY -= 1;
			ZAH[j].x += ZAH[j].w;
			
		}

		for (int j = 0; j < deleteZAH.size(); j++) // vymazanie hran s deltaY = 0 zo ZAH
			ZAH.removeAt(deleteZAH[j] - j);

		deleteZAH.clear();
		y++;
	}

	update();
}
void ViewerWidget::fillTriangleScanLine(QVector<QPoint> T, int interpolationMethod)
{
	//qDebug() << "triangle scanLine";
	QPoint P(-1, -1);
	QPoint e1[2], e2[2], e3[2], e4[2];
	double m = 0.0, w1 = 0.0, w2 = 0.0, w3 = 0.0, w4 = 0.0, x1 = 0.0, x2 = 0.0;
	int y = 0, yMax = 0, deltaX = 0; int Px = 0;

	//printPoints(T);
	//qDebug() << "sorting";
	bubbleSortTrianglePoints(T);
	//printPoints(T);

	if (T[0].y() == T[1].y()) // vlastne spodny trojuholnik -> vodorovna strana je hore
	{
		e1[0] = T[0]; e1[1] = T[2]; // lava hrana
		e2[0] = T[1]; e2[1] = T[2]; // prava hrana
	}
	else if (T[1].y() == T[2].y()) // vlastne horny trojuholnik -> vodorovna strana je dole
	{
		e1[0] = T[0]; e1[1] = T[1]; // lava hrana
		e2[0] = T[0]; e2[1] = T[2]; // prava hrana
	}
	else // trojuholnik treba rozdelit na 2 casti
	{
		m = ((double)T[2].y() - (double)T[0].y()) / ((double)T[2].x() - (double)T[0].x());
		Px = static_cast<int>((((double)T[1].y() - (double)T[0].y()) / m) + T[0].x() + 0.5);

		P.setX(Px); P.setY(T[1].y());

		if (T[1].x() < P.x()) // deliaci bod P je vpravo
		{
			// vrchna cast
			e1[0] = T[0]; e1[1] = T[1];
			e2[0] = T[0]; e2[1] = P;

			// spodna cast
			e3[0] = T[1]; e3[1] = T[2];
			e4[0] = P; e4[1] = T[2];
		}
		else if (T[1].x() > P.x()) // deliaci bod P je nalavo
		{
			// vrchna cast
			e1[0] = T[0]; e1[1] = P;
			e2[0] = T[0]; e2[1] = T[1];

			// spodna cast
			e3[0] = P; e3[1] = T[2];
			e4[0] = T[1]; e4[1] = T[2];
		}
	}

	// smernica a obratena smernica hrany e1
	m = ((double)e1[1].y() - (double)e1[0].y()) / ((double)e1[1].x() - (double)e1[0].x());
	w1 = 1.0 / m;
	// smernica a obratena smernica hrany e2
	m = ((double)e2[1].y() - (double)e2[0].y()) / ((double)e2[1].x() - (double)e2[0].x());
	w2 = 1.0 / m;

	if (P != QPoint(-1, -1)) // ak sa trojuholnik delil, tak sa vypocitaju obratene smernice pre spodnu cast
	{
		m = ((double)e3[1].y() - (double)e3[0].y()) / ((double)e3[1].x() - (double)e3[0].x());
		w3 = 1.0 / m;
		// smernica a obratena smernica hrany e2
		m = ((double)e4[1].y() - (double)e4[0].y()) / ((double)e4[1].x() - (double)e4[0].x());
		w4 = 1.0 / m;
	}

	y = e1[0].y(); // zaciatocny bod usecky e1 -> y suradnica
	yMax = e1[1].y(); // koncovy bod usecky e1 -> y suradnica
	x1 = (double)e1[0].x(); x2 = (double)e2[0].x();

	while (y < yMax)
	{
		deltaX = static_cast<int>(x2) - static_cast<int>(x1);
		
		//if ((x1 - x2) > 0.000000001)
			for (int i = 1; i <= deltaX; i++)
			{
				if (interpolationMethod == NearestNeighbor)
					setPixel(static_cast<int>(x1) + i, y, getNearestNeighborColor(T, QPoint(static_cast<int>(x1) + i, y)));
				else if (interpolationMethod == Barycentric1)
					setPixel(static_cast<int>(x1) + i, y, getBarycentricColor(T, QPoint(static_cast<int>(x1) + i, y)));
				else if (interpolationMethod == Barycentric2)
					setPixel(static_cast<int>(x1) + i, y, getBarycentricDistanceColor(T, QPoint(static_cast<int>(x1) + i, y)));
			}

		x1 += w1; x2 += w2;
		y++;
	}

	if (P != QPoint(-1, -1)) // ak sa rozdeloval trojuhlnik, tak sa vyfarbi aj spodna cast
	{
		y = e3[0].y();
		yMax = e3[1].y();
		x1 = (double)e3[0].x(); x2 = (double)e4[0].x();

		while (y < yMax)
		{
			deltaX = static_cast<int>(x2) - static_cast<int>(x1);

			//if ((x1 - x2) > 0.000000001)
				for (int i = 1; i <= deltaX; i++)
				{
					if (interpolationMethod == NearestNeighbor)
						setPixel(static_cast<int>(x1) + i, y, getNearestNeighborColor(T, QPoint(static_cast<int>(x1) + i, y)));
					else if (interpolationMethod == Barycentric1)
						setPixel(static_cast<int>(x1) + i, y, getBarycentricColor(T, QPoint(static_cast<int>(x1) + i, y)));
					else if (interpolationMethod == Barycentric2)
						setPixel(static_cast<int>(x1) + i, y, getBarycentricDistanceColor(T, QPoint(static_cast<int>(x1) + i, y)));
				}

			x1 += w3; x2 += w4;
			y++;
		}
	}
}

void ViewerWidget::drawHermitCurve(QVector<QPoint> curvePoints, QVector<TangentVector> tangentVectors, QColor color)
{
	QPointF Q0, Q1, T0, T1; // T0 -> dotycnica P'_{i-1}, T1 -> dotycnica P'_{i}
	QPoint startPoint, endPoint;
	double deltaT = 0.05, t = 0.0, angle = 0.0;

	angle = (tangentVectors[0].angle * M_PI) / 180.0;

	// dotykovy vektor prveho bodu
	T0.setX(qSin(angle) * tangentVectors[0].length * 2.0);
	T0.setY(qCos(angle) * tangentVectors[0].length * 2.0);

	for (int i = 1; i < curvePoints.size(); i++)
	{
		t = deltaT;
		Q0 = (QPointF)curvePoints[i - 1];

		angle = (tangentVectors[i].angle * M_PI) / 180.0;

		// dotykovy vektor
		T1.setX(qSin(angle) * tangentVectors[i].length * 2.0);
		T1.setY(qCos(angle) * tangentVectors[i].length * 2.0);

		while (t < 1.0)
		{
			Q1 = (QPointF)curvePoints[i - 1] * F0(t) + (QPointF)curvePoints[i] * F1(t) + T0 * F2(t) + T1 * F3(t);

			// zaokruhlenie suradnic
			startPoint.setX(static_cast<int>(Q0.x() + 0.5)); startPoint.setY(static_cast<int>(Q0.y() + 0.5));
			endPoint.setX(static_cast<int>(Q1.x() + 0.5)); endPoint.setY(static_cast<int>(Q1.y() + 0.5));

			drawLineBresenham(startPoint, endPoint, color); // vykreslenie spojnice

			Q0 = Q1;
			
			t += deltaT;
		}

		startPoint.setX(static_cast<int>(Q0.x() + 0.5)); startPoint.setY(static_cast<int>(Q0.y() + 0.5));

		drawLineBresenham(startPoint, curvePoints[i], color);

		T0 = T1; // dalsia prva dotycnica bude vlastne druha stara
	}
	
}
void ViewerWidget::drawTangentVectors(QVector<QPoint> curvePoints, QVector<TangentVector> tangentVectors, QColor color)
{
	QPoint startPoint, endPoint;
	double angle = 0.0;

	for (int i = 0; i < tangentVectors.size(); i++)
	{
		startPoint = curvePoints[i]; // zaciatocny bod bude bod na krivke
		angle = (tangentVectors[i].angle * M_PI) / 180.0; // premena na radiany

		// dotykovy vektor bude pociatocne nastaveny smerom dole, trochu mensie ako by realne mali byt
		endPoint.setX(qSin(angle) * tangentVectors[i].length / 2.0 + startPoint.x());
		endPoint.setY(qCos(angle) * tangentVectors[i].length / 2.0 + startPoint.y());

		drawLineBresenham(startPoint, endPoint, color);

		QPen pen; pen.setColor(color); painter->setPen(pen);
		painter->drawText(QPoint(endPoint.x() + 5, endPoint.y() - 5), QString("t_%1").arg(i));
	}
}
void ViewerWidget::drawBezierCurve(QVector<QPoint> curvePoints, QColor color)
{
	int n = curvePoints.size(), size = 0;
	QPointF** P = new QPointF* [n];
	QPointF Q0, Q1;
	QPoint startPoint, endPoint;
	double deltaT = 0.05, t = 0.0;

	for (int i = 0; i < n; i++)
	{
		size = n - i;
		P[i] = new QPointF[size];
		P[0][i] = (QPointF)curvePoints[i]; // vkladanie bodov P_0, P_1, P_2, ..., P_{curvePoints.size - 1} do prveho riadku
	}

	t = deltaT;

	Q0 = (QPointF)curvePoints[0]; // prvy bod segmentu

	do
	{
		for (int i = 1; i < n; i++)
		{
			for (int j = 0; j < (n - i); j++)
			{
				P[i][j] = (1.0 - t) * P[i - 1][j] + t * P[i - 1][j + 1];
			}
		}

		Q1 = P[n - 1][0];

		// zaokruhlenie suradnic
		startPoint.setX(static_cast<int>(Q0.x() + 0.5)); startPoint.setY(static_cast<int>(Q0.y() + 0.5));
		endPoint.setX(static_cast<int>(Q1.x() + 0.5)); endPoint.setY(static_cast<int>(Q1.y() + 0.5));

		drawLineBresenham(startPoint, endPoint, color); // vykreslenie spojnice

		Q0 = Q1;

		t += deltaT;
	} while (t < 1.0);

	startPoint.setX(static_cast<int>(Q0.x() + 0.5)); startPoint.setY(static_cast<int>(Q0.y() + 0.5));

	drawLineBresenham(startPoint, curvePoints[curvePoints.size() - 1], color);

	// uvolnenie alokovanej memory
	for (int i = 0; i < n; i++)
		delete[] P[i];

	delete[] P;
}
void ViewerWidget::drawCoonsCurve(QVector<QPoint> curvePoints, QColor color)
{
	QPointF Q0, Q1;
	QPoint startPoint, endPoint;
	double deltaT = 0.05, t = 0.0;

	for (int i = 3; i < curvePoints.size(); i++)
	{
		t = 0.0;

		Q0 = (QPointF)curvePoints[i - 3] * B0(0) + (QPointF)curvePoints[i - 2] * B1(0) + (QPointF)curvePoints[i - 1] * B2(0) + (QPointF)curvePoints[i] * B3(0);

		do
		{
			t += deltaT;

			Q1 = (QPointF)curvePoints[i - 3] * B0(t) + (QPointF)curvePoints[i - 2] * B1(t) + (QPointF)curvePoints[i - 1] * B2(t) + (QPointF)curvePoints[i] * B3(t);

			startPoint.setX(static_cast<int>(Q0.x() + 0.5)); startPoint.setY(static_cast<int>(Q0.y() + 0.5));
			endPoint.setX(static_cast<int>(Q1.x() + 0.5)); endPoint.setY(static_cast<int>(Q1.y() + 0.5));

			drawLineBresenham(startPoint, endPoint, color);

			Q0 = Q1;
		} while (t < 1.0);
	}
}

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Data function
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (isInside(x, y)) {
		data[x + y * img->width()] = color.rgb();
	}
}
void ViewerWidget::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if (isInside(x, y)) {
		QColor color(r, g, b);
		setPixel(x, y, color);
	}
}

void ViewerWidget::clear(QColor color)
{
	for (size_t x = 0; x < img->width(); x++)
	{
		for (size_t y = 0; y < img->height(); y++)
		{
			setPixel(x, y, QColor(45, 45, 45));
		}
	}
	update();
}

void ViewerWidget::drawLineDDA(QPoint point1, QPoint point2, QColor color)
{
	// trochu prerobeny DDA algoritmus podla: http://www.st.fmph.uniba.sk/~vojtova5/PG/Rast-use-DDA.html
	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();
	int steps = 0;

	double xIncrement = 0.0, yIncrement = 0.0;
	double x = point1.x(), y = point1.y();

	if (qAbs(deltaX) > qAbs(deltaY))
		steps = qAbs(deltaX);
	else
		steps = qAbs(deltaY);

	xIncrement = deltaX / static_cast<double>(steps);
	yIncrement = deltaY / static_cast<double>(steps);

	setPixel(static_cast<int>(x), static_cast<int>(y), color);

	for (int i = 0; i < steps; i++)
	{
		x += xIncrement;
		y += yIncrement;

		setPixel(static_cast<int>(x), static_cast<int>(y), color);
	}
	
	update();
}
void ViewerWidget::drawLineBresenham(QPoint point1, QPoint point2, QColor color)
{
	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();
	double slope = static_cast<double>(deltaY) / static_cast<double>(deltaX);

	if ((slope >= 0 && slope < 1) || (slope > -1 && slope <= 0)) // riadiaca os x
		drawBresenhamChosenX(point1, point2, color);
	else if ((slope >= 1) || (slope <= -1)) // riadiaca os y
		drawBresenhamChosenY(point1, point2, color);
	
	update();
}
void ViewerWidget::createLineWithAlgorithm(QPoint point1, QPoint point2, QColor color, int lineAlgorithm, bool shouldDrawPoints)
{
	//qDebug() << "line drawn:" << point1 << point2;
	if (lineAlgorithm == 0) // DDA
	{
		drawLineDDA(point1, point2, color);
		//painter->drawText(point1, QString("(%1,%2)").arg(point1.x()).arg(point1.y()));
		//painter->drawText(point2, QString("(%1,%2)").arg(point2.x()).arg(point2.y()));
		if (shouldDrawPoints)
		{
			drawPoint(point1, defaultColor0);
			drawPoint(point2, defaultColor0);
		}
	}
	else if (lineAlgorithm == 1) // mr. Bresenham
	{
		drawLineBresenham(point1, point2, color);
		//painter->drawText(point1, QString("(%1,%2)").arg(point1.x()).arg(point1.y()));
		//painter->drawText(point2, QString("(%1,%2)").arg(point2.x()).arg(point2.y()));
		if (shouldDrawPoints)
		{
			drawPoint(point1, defaultColor0);
			drawPoint(point2, defaultColor0);
		}
	}
	else
		qDebug() << "Incorrect lineAlgorithm";
}
void ViewerWidget::drawCircumference(QPoint point1, QPoint point2, QColor color)
{
	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();

	int radius = static_cast<int>(qSqrt(static_cast<double>(deltaX) * deltaX + static_cast<double>(deltaY) * deltaY));
	int p = 1 - radius;
	int x = 0;
	int y = radius;
	int dvaX = 3;
	int dvaY = 2 * radius + 2;
	int Sx = point1.x();
	int Sy = point1.y();

	while (x <= y)
	{
		// pixely hore
		setPixel(Sx - y, Sy + x, color);
		setPixel(Sx - y, Sy - x, color);
		
		// pixely dole
		setPixel(Sx + y, Sy + x, color);
		setPixel(Sx + y, Sy - x, color);

		// pixely vpravo
		setPixel(Sx + x, Sy - y, color);
		setPixel(Sx + x, Sy + y, color);

		// pixely na lavo
		setPixel(Sx - x, Sy - y, color);
		setPixel(Sx - x, Sy + y, color);

		if (p > 0)
		{
			p -= dvaY;
			y--;
			dvaY -= 2;
		}

		p += dvaX;
		dvaX += 2;
		x++;
	}

	update();
}
void ViewerWidget::drawPoint(QPoint point, QColor color)
{
	int size = 3;
	
	//drawCircumference(point, QPoint(point.x() + size, point.y()), color);

	drawLineBresenham(QPoint(point.x() - size, point.y() - size), QPoint(point.x() + size, point.y() + size), color);
	drawLineBresenham(QPoint(point.x() - size, point.y() + size), QPoint(point.x() + size, point.y() - size), color);
	drawLineBresenham(QPoint(point.x(), point.y() - size), QPoint(point.x(), point.y() + size), color);
	drawLineBresenham(QPoint(point.x() - size, point.y()), QPoint(point.x() + size, point.y()), color);
}
void ViewerWidget::drawPoints(QVector<QPoint> points, QColor color)
{
	for (int i = 0; i < points.size(); i++)
		drawPoint(points[i], color);
}

void ViewerWidget::createGeometry(QVector<QPoint>& geometryPoints, QColor penColor, QColor fillColor, int lineAlgorithm, int interpolationMethod)
{
	if (geometryPoints.size() == 2)
		trimLine(geometryPoints, penColor, lineAlgorithm);
	else if (geometryPoints.size() > 2)
		trimPolygon(geometryPoints, penColor, fillColor, lineAlgorithm, interpolationMethod);
}

void ViewerWidget::createCurve(QVector<QPoint>& curvePoints, QVector<TangentVector> tangentVectors, QColor color, int curveType)
{
	drawPoints(curvePoints, defaultColor2); // body

	if (curveType == CurveType::HermitCurve)
	{
		drawHermitCurve(curvePoints, tangentVectors, color); // krivka
		drawTangentVectors(curvePoints, tangentVectors, defaultColor1); // dotykove vektory
	}
	else if (curveType == CurveType::BezierCurve)
		drawBezierCurve(curvePoints, color); // krivka
	else if (curveType == CurveType::CoonsCurve)
		drawCoonsCurve(curvePoints, color); // krivka
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}