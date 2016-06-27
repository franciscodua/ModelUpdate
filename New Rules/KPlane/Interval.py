def is_overlapping(interval1, interval2):
    return max(interval1[0], interval2[0]) < min(interval1[1], interval2[1])


def merge_range(range1, interval, dimension):
    new_interval = []
    for i in range(len(interval.ranges)):
        if i == dimension:
            new_interval.append(range1)
        else:
            new_interval.append(interval.ranges[i])

    return Interval(new_interval)


def find_new_intervals(interval1, interval2, dimension):
    new_intervals = []
    new_ranges = []

    range1 = interval1.ranges[dimension]
    range2 = interval2.ranges[dimension]

    limits = list(set(list(range1) + list(range2)))
    limits.sort()

    for i in range(len(limits) - 1):
        new_ranges.append((limits[i], limits[i + 1]))

    # when input space has one dimension
    if len(interval1.ranges) == 1:
        return [Interval([new_range]) for new_range in new_ranges]

    for new_range in new_ranges:
        if is_overlapping(new_range, range1):
            new_intervals.append(merge_range(new_range, interval1, dimension))
        if is_overlapping(new_range, range2):
            new_intervals.append(merge_range(new_range, interval2, dimension))

    return new_intervals


def split_intervals(list_intervals):
    n_dimensions = len(list_intervals[0].ranges)
    result = []

    for dimension in range(n_dimensions):
        result = []
        while list_intervals:
            # POP
            first_element = list_intervals[0]
            del list_intervals[0]
            append_to_result = True
            index = 0
            while index < len(list_intervals):
                second_element = list_intervals[index]
                # TODO: use one condition
                if first_element.intersects(second_element):
                    if first_element.ranges[dimension] != second_element.ranges[dimension]:
                        del list_intervals[index]
                        new_intervals = find_new_intervals(first_element, second_element, dimension)
                        list_intervals.extend(new_intervals)
                        append_to_result = False
                        break
                index += 1
            if append_to_result and first_element.ranges not in [x.ranges for x in result]:
                result.append(first_element)
        list_intervals = [Interval(interval.ranges) for interval in result]
    return result


class Interval:
    """
    Intervals which define the valid hypercube
    """

    def __init__(self, ranges):
        """
        :param ranges: list with a pair for each dimension, where the first element is the minimum value and
        the second the maximum
        """
        self.ranges = ranges

    def intersects(self, interval):
        """
        Predicate that indicates if every dimension overlaps (hypercubes intersect)
        :param interval: Object of type Interval
        :return: True if intervals intersect, False otherwise
        """
        for dimension in range(len(self.ranges)):
            if not is_overlapping(self.ranges[dimension], interval.ranges[dimension]):
                return False
        return True

    def belongs(self, point):
        for dimension in range(len(self.ranges)):
            if not self.ranges[dimension][0] <= point.coords[dimension] < self.ranges[dimension][1]:
                return False
        return True
