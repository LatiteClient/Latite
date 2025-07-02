#include "pch.h"
#include "WaypointManager.h"

void WaypointManager::addWaypoint(const WaypointData& waypoint) {
    waypoints.push_back(waypoint);
}

void WaypointManager::removeWaypoint(const WaypointData& waypoint) {
    waypoints.erase(std::remove_if(waypoints.begin(), waypoints.end(),
        [&waypoint](const WaypointData& wp) {
            return wp.position == waypoint.position &&
                wp.dimension == waypoint.dimension &&
                wp.initials == waypoint.initials &&
                wp.name == waypoint.name; // colors can be the same so it's not in the criteria here
        }),
        waypoints.end());

    smoothedPositions.erase(waypoint.position);
}

const std::vector<WaypointData>& WaypointManager::getWaypoints() const {
    return waypoints;
}

Vec2 WaypointManager::getSmoothedPosition(const Vec3& key, const Vec2& target, float t) {
    Vec2& smoothed = smoothedPositions[key];

    if (smoothed.x == 0.0f && smoothed.y == 0.0f) {
        smoothed = target;
    } else {
        smoothed.x = std::lerp(smoothed.x, target.x, t);
        smoothed.y = std::lerp(smoothed.y, target.y, t);
    }

    return smoothed;
}
