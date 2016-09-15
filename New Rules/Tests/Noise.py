import sys

import util
from KPlane import kplane


def test():
    data_file = sys.argv[1]

    dataset = util.read_points_csv(data_file)

    for i in range(5):
        k = kplane.choose_k(dataset)
        print k
