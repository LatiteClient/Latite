#include "pch.h"
#include "WaypointManager.h"

void WaypointManager::addWaypoint(const WaypointData& waypoint) {
    waypoints.push_back(waypoint);
}

void WaypointManager::removeWaypoint(const Vec3& position) {
    std::vector<WaypointData>::iterator newEnd = std::ranges::remove_if(
        waypoints,
        [&position](const WaypointData& waypoint) {
            return waypoint.position == position;
        }
    ).begin();

    waypoints.erase(newEnd, waypoints.end());

    smoothedPositions.erase(position);
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
