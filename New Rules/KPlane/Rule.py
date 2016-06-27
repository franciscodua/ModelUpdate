from ImpactFunction import ImpactFunction


class Rule:
    def __init__(self, interval, functions, points):
        """
        Initialize Rule with validity interval, impact functions and probabilities
        :param interval: Interval object that represents validity interval
        :param functions: list of impact functions (objects Function)
        :param points: list of points used to compute probability
        """
        self.impact_functions = []
        self.interval = interval

        possible_impacts = [ImpactFunction(function) for function in functions]
        number_of_points = 0

        for point in points:
            if self.interval.belongs(point):
                number_of_points += 1

                best_impact_index = 0
                best_impact_error = possible_impacts[0].function.get_error(point)
                for i in range(1, len(possible_impacts)):
                    impact_error = possible_impacts[i].function.get_error(point)
                    if impact_error < best_impact_error:
                        best_impact_index = i
                        best_impact_error = impact_error

                possible_impacts[best_impact_index].points.append(point)

        for impact_function in possible_impacts:
            impact_number_points = len(impact_function.points)
            if impact_number_points > 0:
                impact_function.probability = impact_number_points / float(number_of_points)
                self.impact_functions.append(impact_function)

    def predict(self, point):
        result = 0

        for impact_function in self.impact_functions:
            result += impact_function.probability * impact_function.function.predict(point)

        return result
