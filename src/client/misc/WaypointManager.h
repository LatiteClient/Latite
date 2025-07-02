#pragma once

struct WaypointData {
    std::wstring name;
    std::wstring initials;
    Color color;
    Vec3 position;
    std::wstring dimension;
};

class WaypointManager {
public:
    const std::vector<WaypointData>& getWaypoints() const;
    void addWaypoint(const WaypointData& waypoint);
    void removeWaypoint(const WaypointData& waypoint);

    Vec2 getSmoothedPosition(const Vec3& key, const Vec2& target, float t);

private:
    std::vector<WaypointData> waypoints;
    std::unordered_map<Vec3, Vec2, Vec3Hasher> smoothedPositions;
};
