import math
import numpy
from sklearn import linear_model

from objects import Function


class Cluster:
    """
    A set of points and their centroid
    """

    def __init__(self, points):
        """
        :param points: A list of point objects
        """

        if len(points) == 0:
            raise Exception("ILLEGAL: empty cluster")

        # The points that belong to this cluster
        self.points = points

        # The dimensionality of the points in this cluster
        self.n = points[0].n

        # Assert that all points are of the same dimensionality
        for p in points:
            if p.n != self.n:
                raise Exception("ILLEGAL: wrong dimensions")

        # Set up the initial centroid (this is usually based off one point)
        self.centroid = self.calculate_centroid_function()

        self.accuracy = 0

    def __repr__(self):
        return str(self.points)

    def update(self, points):
        """
        Returns the distance between the previous centroid and the new after
        recalculating and storing the new centroid.
        :param points: list of points
        :return: euclidean distance between new and old centroids
        """
        old_centroid = self.centroid
        self.points = points
        self.centroid = self.calculate_centroid_function()
        self.accuracy = self.centroid.compute_fit_accuracy(self.points)
        shift = self.centroid.get_distance(old_centroid)
        return shift

    def calculate_centroid_function(self):
        """
        Computes linear function based on points.
        :return: Function that represents new centroid
        """
        num_points = len(self.points)
        input_size = self.points[0].n - 1
        # Get a list of all coordinates in this cluster
        coords = [p.coords for p in self.points]
        coords_array = numpy.array(coords)
        output_array = coords_array[:, input_size]
        input_array = coords_array[:, range(input_size)]
        # fir requirement
        output_array = output_array.reshape(num_points, 1)
        input_array = input_array.reshape(num_points, input_size)

        regr = linear_model.LinearRegression()

        regr.fit(input_array, output_array)

        point_coords = [x[0] for x in regr.coef_.tolist()]
        point_coords.append(regr.intercept_.tolist()[0])

        return Function(point_coords)