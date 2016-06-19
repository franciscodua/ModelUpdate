import csv

from KPlane.objects import Point


def avg(lst):
    return sum(lst) / len(lst)


def read_points_csv(input_file):
    """
    Reads points from CSV file and creates Point for each line
    :param input_file: string with file name
    :return: list of points
    """
    points = []
    with open(input_file) as csv_file:
        reader = csv.reader(csv_file, delimiter=',')
        for row in reader:
            points.append(Point([float(x) for x in row]))
    return points
