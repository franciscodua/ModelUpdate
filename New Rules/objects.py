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

        ret = reduce(lambda x,y: x + pow((self.coords[y]-point.coords[y]), 2), range(self.n), 0.0)
        return math.sqrt(ret)


class Function(Point):
    """
    A linear function represented as a point in space.
    coords are weights and interception
    """

    def __predict(self, point):
        if self.n != point.n:
            raise Exception("ILLEGAL: Function and points should have same dim")
        input_size = self.n - 1
        # intersection
        prediction = self.coords[input_size]
        for i in range(input_size):
            # weight_i X point_i
            prediction += (self.coords[i] * point.coords[i])
        return prediction

    def __total_sum_squares(self, points):
        """
        :param points: list of points
        :return: total sum squares (sum(y_i - meanY)^2)
        """
        input_size = len(points[0].coords) - 1
        y = [p.coords[input_size] for p in points]
        # (1/n)sum(y_i)
        mean_y = sum(y) / len(y)
        result = 0
        for y_i in y:
            result += (y_i - mean_y) ** 2
        return result

    def __residual_sum_squares(self, points):
        """
        :param points: list of points
        :return: residual sum squares (sum(y_i - f_i)^2)
        """
        result = 0
        for i in range(len(points)):
            result += (self.get_error(points[i])) ** 2
        return result

    def get_error(self, point):
        """
        Prediction error of point using function
        Note: Last item in self.coords is intersection point.
        Note: Last item in point is considered output
        """
        if self.n != point.n:
            raise Exception("ILLEGAL: Function and points should have same dim")
        output_index = self.n - 1
        prediction = self.__predict(point)
        # error
        return abs(prediction - point.coords[output_index])

    def compute_fit_error(self, points):
        output_index = self.n - 1
        error = 0
        for p in points:
            prediction = self.__predict(p) 
            error += abs(prediction - p.coords[output_index]) / (abs(prediction) + 0.0000001)
        return error / len(points)

    def compute_fit_accuracy(self, points):
        # total sum of squares
        ss_tot = self.__total_sum_squares(points)
        # residual sum of squares
        ss_res = self.__residual_sum_squares(points)
        # coefficient of determination (R^2)
        return 1 - (ss_res / ss_tot)

