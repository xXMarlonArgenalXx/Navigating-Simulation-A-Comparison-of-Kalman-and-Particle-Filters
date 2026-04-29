#include <iostream>
#include <vector>
#include <random>
#include <cmath>

using namespace std;

struct Position {
    double x;
    double y;
};

double distance(Position a, Position b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

int main() {
    default_random_engine generator;
    normal_distribution<double> noise(0.0, 1.0);

    Position truePos = {0, 0};
    Position velocity = {1, 1};

    Position kalmanEstimate = {0, 0};

    vector<Position> particles(100);
    for (auto &p : particles) {
        p = {noise(generator), noise(generator)};
    }

    cout << "Step,TrueX,TrueY,MeasuredX,MeasuredY,KalmanX,KalmanY,ParticleX,ParticleY\n";

    for (int step = 1; step <= 50; step++) {
        truePos.x += velocity.x;
        truePos.y += velocity.y;

        Position measurement = {
            truePos.x + noise(generator),
            truePos.y + noise(generator)
        };

        // Simple Kalman-style update
        double kalmanGain = 0.5;
        kalmanEstimate.x = kalmanEstimate.x + velocity.x;
        kalmanEstimate.y = kalmanEstimate.y + velocity.y;

        kalmanEstimate.x = kalmanEstimate.x + kalmanGain * (measurement.x - kalmanEstimate.x);
        kalmanEstimate.y = kalmanEstimate.y + kalmanGain * (measurement.y - kalmanEstimate.y);

        // Simple Particle Filter
        Position particleEstimate = {0, 0};

        for (auto &p : particles) {
            p.x += velocity.x + noise(generator) * 0.2;
            p.y += velocity.y + noise(generator) * 0.2;
        }

        double totalWeight = 0;

        vector<double> weights;
        for (auto &p : particles) {
            double d = distance(p, measurement);
            double weight = 1.0 / (d + 0.001);
            weights.push_back(weight);
            totalWeight += weight;
        }

        for (int i = 0; i < particles.size(); i++) {
            weights[i] /= totalWeight;
            particleEstimate.x += particles[i].x * weights[i];
            particleEstimate.y += particles[i].y * weights[i];
        }

        cout << step << ","
             << truePos.x << "," << truePos.y << ","
             << measurement.x << "," << measurement.y << ","
             << kalmanEstimate.x << "," << kalmanEstimate.y << ","
             << particleEstimate.x << "," << particleEstimate.y << endl;
    }

    return 0;
}
