#!/usr/bin/env python3

class Kalman:
    def __init__(self, r, q):
        # r=1.4, q=0.065 values recomended for BLE applications
        self.r = r          # Noιse related to sensor
        self.q = q          # Noise related to process
        self.p = 0          # Estimated error
        self.k = 0          # Kalman gain
        self.value = 0.0    # Predicted value
        self.first = True   # First iteration flag

    def update(self, measurement):
        # Prediction
        self.p = self.p + self.q
        # Correction
        self.k = self.p / (self.p + self.r)
        if self.first: 
            self.value = measurement # First time prediction and value are the same
            self.first = False
        else: self.value = self.value + self.k * (measurement - self.value)
        self.p = (1 - self.k) * self.p
        return self.value