import math


class Point:
    """
    A point in n dimensional space
    """

    def __init__(self, coords):
        """
        :param coords: A list of values, one per dimension
        """

        self.coords = coords
        self.n = len(coords)

    def __repr__(self):
        return str(self.coords)

    def get_distance(self, point):
        """
        Euclidean distance between two n-dimensional points.
        Note: This can be very slow and does not scale well
        :param point: point to which distance should be computed
        :return: euclidean distance between self to point
        """
        if self.n != point.n:
            raise Exception("ILLEGAL: non comparable points")

        return math.sqrt(sum([(self.coords[i] - point.coords[i]) ** 2 for i in range(self.n)]))


class Function(Point):
    """
    A linear function represented as a point in space.
    coords are weights and interception
    """

    def predict(self, point):
        if self.n != point.n:
            raise Exception("ILLEGAL: Function and points should have same dim")
        input_size = self.n - 1
        # intersection
        prediction = self.coords[input_size]
        for i in range(input_size):
            # weight_i X point_i
            prediction += (self.coords[i] * point.coords[i])
        return prediction

    def get_error(self, point):
        """
        Prediction error of point using function
        Note: Last item in self.coords is intersection point.
        Note: Last item in point is considered output
        :param point: point object to which error should be computed
        """
        if self.n != point.n:
            raise Exception("ILLEGAL: Function and points should have same dim")
        output_index = self.n - 1
        prediction = self.predict(point)
        # error
        return abs(prediction - point.coords[output_index])

    def compute_fit_error(self, points):
        output_index = self.n - 1
        error = 0
        for p in points:
            prediction = self.predict(p)
            error += abs(prediction - p.coords[output_index]) / (abs(prediction) + 0.0000001)
        return error / len(points)

