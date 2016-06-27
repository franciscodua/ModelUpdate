class ImpactFunction:
    def __init__(self, function):
        """
        Impact Function has a Function and a set of points that belong to this impact
        :param function: Function
        """
        self.function = function
        self.points = []
        self.probability = 0
