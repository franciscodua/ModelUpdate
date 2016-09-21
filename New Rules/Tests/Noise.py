import util
from KPlane import kplane


def test():
    for i in range(5, 51, 5):
        data_file = '/Users/francisco/Documents/IST/Tests/Noise/noise-' + str(i) + '.csv'
        # data_file = '/Users/francisco/Documents/IST/Tests/Noise/clean.csv'

        dataset = util.read_points_csv(data_file)
        print i
        for j in range(10):
            k = kplane.choose_k(dataset)
            print '\t' + str(k),
        print
