#############################################################################
# Full Imports

import sys
import math
import random
import subprocess
import csv
import numpy
from sklearn import linear_model

"""
This is a pure Python implementation of the K-Means Clustering algorithmn. The
original can be found here:
http://pandoricweb.tumblr.com/post/8646701677/python-implementation-of-the-k-means-clustering

I have refactored the code and added comments to aid in readability.
After reading through this code you should understand clearly how K-means works.
If not, feel free to email me with questions and suggestions. (iandanforth at
gmail)

This script specifically avoids using numpy or other more obscure libraries. It
is meant to be *clear* not fast.

"""
# DEBUG should be true if prints are wanted
DEBUG = False
def main():
    # Generate some points
    file = sys.argv[1]
    points = readPointsCsv(file)
    
    # How many points are in our dataset?
    num_points = len(points)
    
    # For each of those points how many dimensions do they have?
    dimensions = points[0].n
    
    # The K in k-means. How many clusters do we assume exist?
    # should receive as input
    num_clusters = 4
    
    # When do we say the optimization has 'converged' and stop updating clusters
    opt_cutoff = 0.5

    # Cluster those data!
    #clusters = kmeans(points, num_clusters, opt_cutoff)
    clusters = fitFunctions(points, opt_cutoff)

    for c in clusters:
        print "Cluster: " + str(c.centroid) + ". Accuracy: " + str(c.error)
    # Print our clusters
    #for i,c in enumerate(clusters):
     #   for p in c.points:
      #      print " Cluster: ", i, "\t Point :", p

class Point:
    '''
    An point in n dimensional space
    '''
    def __init__(self, coords):
        '''
        coords - A list of values, one per dimension
        '''
        
        self.coords = coords
        self.n = len(coords)
        
    def __repr__(self):
        return str(self.coords)

class Cluster:
    '''
    A set of points and their centroid
    '''
    
    def __init__(self, points):
        '''
        points - A list of point objects
        '''
        
        if len(points) == 0: raise Exception("ILLEGAL: empty cluster")
        # The points that belong to this cluster
        self.points = points
        
        # The dimensionality of the points in this cluster
        self.n = points[0].n
        
        # Assert that all points are of the same dimensionality
        for p in points:
            if p.n != self.n: raise Exception("ILLEGAL: wrong dimensions")
            
        # Set up the initial centroid (this is usually based off one point)
        self.centroid = self.calculateCentroidFunction()

        self.error = 0
        
    def __repr__(self):
        '''
        String representation of this object
        '''
        return str(self.points)
    
    def update(self, points):
        '''
        Returns the distance between the previous centroid and the new after
        recalculating and storing the new centroid.
        '''
        old_centroid = self.centroid
        self.points = points
        self.centroid = self.calculateCentroidFunction()
        self.error = computeFitError(self.centroid, self.points)
        shift = getDistance(old_centroid, self.centroid)
        return shift
    
    def calculateCentroid(self):
        '''
        Finds a virtual center point for a group of n-dimensional points
        '''
        numPoints = len(self.points)
        # Get a list of all coordinates in this cluster
        coords = [p.coords for p in self.points]
        # Reformat that so all x's are together, all y'z etc.
        unzipped = zip(*coords)
        # Calculate the mean for each dimension
        centroid_coords = [math.fsum(dList)/numPoints for dList in unzipped]
        
        return Point(centroid_coords)

    def calculateCentroidFunction(self):
        '''
        Computes linear function based on points.
        Note: returns Point (compatibility)
        '''
        #if not self.points:
            #return self.centroid
        numPoints = len(self.points)
        inputDim = self.points[0].n - 1
        # Get a list of all coordinates in this cluster
        coords = [p.coords for p in self.points]
        coords_a = numpy.array(coords)
        output_a = coords_a[:, inputDim]
        input_a = coords_a[:, range(inputDim)]
        # fir requirement
        output_a = output_a.reshape(numPoints, 1)
        input_a = input_a.reshape(numPoints, inputDim)

        regr = linear_model.LinearRegression()

        regr.fit(input_a, output_a)

        pCoords = [x[0] for x in regr.coef_.tolist()]
        pCoords.append(regr.intercept_.tolist()[0])

        return Point(pCoords)

def kmeans(points, k, cutoff):
    
    # Pick out k random points to use as our initial centroids
    dim = points[0].n
    initial = random.sample(points, k*dim)
    
    # Create k clusters using those centroids
    clusters = [Cluster(random.sample(points, dim)) for i in range(k)]
    #clusters = [Cluster([p]) for p in initial]
    
    # Loop through the dataset until the clusters stabilize
    loopCounter = 0
    while True:
        # Create a list of lists to hold the points in each cluster
        lists = [ [] for c in clusters]
        clusterCount = len(clusters)
        
        # Start counting loops
        loopCounter += 1
        # For every point in the dataset ...
        for p in points:
            # Get the distance between that point and the centroid of the first
            # cluster.
            smallest_distance = getError(clusters[0].centroid, p)
        
            # Set the cluster this point belongs to
            clusterIndex = 0
        
            # For the remainder of the clusters ...
            for i in range(clusterCount - 1):
                # calculate the distance of that point to each other cluster's
                # centroid.
                distance = getError(clusters[i+1].centroid, p)
                # If it's closer to that cluster's centroid update what we
                # think the smallest distance is, and set the point to belong
                # to that cluster
                if distance < smallest_distance:
                    smallest_distance = distance
                    clusterIndex = i+1
            lists[clusterIndex].append(p)

        for lst in lists:
            if len(lst) < dim:
                lst.extend(random.sample(points, dim))
        
        # Set our biggest_shift to zero for this iteration
        biggest_shift = 0.0
        
        # As many times as there are clusters ...
        for i in range(clusterCount):
            # Calculate how far the centroid moved in this iteration
            shift = clusters[i].update(lists[i])
            # Keep track of the largest move from all cluster centroid updates
            biggest_shift = max(biggest_shift, shift)
        
        # If the centroids have stopped moving much, say we're done!
        if biggest_shift < cutoff:
            if DEBUG == True:
                print "Converged after %s iterations" % loopCounter
            break
    return clusters

def fitFunctions(points, cutoff):
    THRESHOLD = 0.05
    N_TIMES = 10
    error = 1
    # A max k must be defined (10 for the moment)
    for k in range(1, 10):
        for i in range(N_TIMES):
            new_clusters = kmeans(points, k, cutoff)
            new_error = computeMeanError(new_clusters)
            if new_error < error:
                error = new_error
                clusters = new_clusters

        if error < THRESHOLD:
            break
    return clusters

def computeMeanError(clusters):
    accuracy = 0
    for c in clusters:
        accuracy += c.error
    return 1 - (accuracy / len(clusters))

def getDistance(a, b):
    '''
    Euclidean distance between two n-dimensional points.
    Note: This can be very slow and does not scale well
    '''
    if a.n != b.n:
        raise Exception("ILLEGAL: non comparable points")
    
    ret = reduce(lambda x,y: x + pow((a.coords[y]-b.coords[y]), 2),range(a.n),0.0)
    return math.sqrt(ret)

def getError(f, p):
    '''
    Prediction error of point p using function f
    f is also represented as point.
    Note: Last item in f.coords is intersection point.
    Note: Last item in p is considered output
    '''
    if f.n != p.n:
        raise Exception("ILLEGAL: Function and points should have same dim")

    inputDim = f.n - 1
    #intersection
    prediction = f.coords[inputDim]
    for i in range(inputDim):
        # weight_i X point_i
        prediction += (f.coords[i] * p.coords[i])
    # error
    return abs(prediction - p.coords[inputDim])

def computeFitError(function, points):
    outputIndex = len(points[0].coords) - 1
    y = [p.coords[outputIndex] for p in points]
    # (1/n)sum(y_i)
    meanY = sum(y) / len(y)
    # total sum of squares
    SS_tot = totalSumSquares(y, meanY)
    # residual sum of squares
    SS_res = residualSumSquares(points, function)
    # coefficient of determination (R^2)
    return 1 - (SS_res / SS_tot)

# sum(y_i - meanY)^2
def totalSumSquares(output, mean):
    result = 0
    for y_i in output:
        result += (y_i - mean) ** 2
    return result

# sum(y_i - f_i)^2
def residualSumSquares(points, function):
    result = 0
    for i in range(len(points)):
        result += (getError(function, points[i])) ** 2
    return result

# Reads points from CSV file and creates Point for each line
def readPointsCsv(file):
    f = open(file)
    points = []
    with f as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            points.append(Point([float(x) for x in row]))

    return points

if __name__ == "__main__": 
    main()