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
                wp.name == waypoint.name; // colors can be the same, so it's not in the criteria here
        }),
        waypoints.end());

    smoothedPositions.erase(waypoint.position);
}

const std::vector<WaypointData>& WaypointManager::getWaypoints() const {
    return waypoints;
}

Vec2 WaypointManager::getSmoothedPosition(const Vec3& key, const Vec2& target) {
    Vec2& previous = smoothedPositions[key];

    float alpha = SDK::ClientInstance::get()->minecraft->timer->alpha;

    if (previous.x == 0.f && previous.y == 0.f) {
        previous = target;
        return target;
    }

    Vec2 interpolated = {
        std::lerp(previous.x, target.x, alpha),
        std::lerp(previous.y, target.y, alpha)
    };

    previous = interpolated;

    return interpolated;
}
