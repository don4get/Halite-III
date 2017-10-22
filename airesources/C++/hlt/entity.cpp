//
// Created by David Li on 7/17/17.
//

#include <cassert>
#include "entity.hpp"

namespace hlt {
    auto Location::distance(const Location &other) const -> double {
        return sqrt(distance2(other));
    }

    auto Location::distance2(const Location &other) const -> double {
        return std::pow(other.pos_x - pos_x, 2) +
            std::pow(other.pos_y - pos_y, 2);
    }

    auto Location::move_by(const Velocity& velocity, double time) -> void {
        pos_x += time * velocity.vel_x;
        pos_y += time * velocity.vel_y;
    }

    auto Location::angle_to(const Location& target) const -> double {
        auto dx = target.pos_x - this->pos_x;
        auto dy = target.pos_y - this->pos_y;

        auto angle_rad = std::atan2(dy, dx);
        if (angle_rad < 0) {
            angle_rad += 2 * M_PI;
        }

        return angle_rad;
    }

    auto operator<<(std::ostream &ostream,
                    const Location &location) -> std::ostream & {
        ostream << '(' << location.pos_x << ", " << location.pos_y << ')';
        return ostream;
    }

    auto Velocity::accelerate_by(double magnitude,
                                 double angle) -> void {
        double new_vel_x = vel_x + magnitude * std::cos(angle);
        double new_vel_y = vel_y + magnitude * std::sin(angle);

        const auto max_speed = constants::MAX_SPEED;
        if (this->magnitude() > max_speed) {
            double scale = max_speed / this->magnitude();
            new_vel_x *= scale;
            new_vel_y *= scale;
        }
        vel_x = new_vel_x;
        vel_y = new_vel_y;
    }

    auto Velocity::magnitude() const -> double {
        return sqrt(vel_x * vel_x + vel_y * vel_y);
    }

    auto Velocity::angle() const -> double {
        return atan2(vel_y, vel_x);
    }

    EntityId::EntityId() {
        type = EntityType::InvalidEntity;
        _player_id = -1;
        _entity_index = -1;
    }

    auto EntityId::is_valid() const -> bool {
        return type != EntityType::InvalidEntity &&
            _player_id >= -1 && _entity_index >= 0;
    }

    auto EntityId::invalid() -> EntityId {
        return EntityId();
    }

    auto EntityId::player_id() const -> PlayerId {
        return static_cast<PlayerId>(_player_id);
    }

    auto EntityId::entity_index() const -> EntityIndex {
        return static_cast<EntityIndex>(_entity_index);
    }

    auto EntityId::for_planet(EntityIndex index) -> EntityId {
        auto result = EntityId();
        result.type = EntityType::PlanetEntity;
        result._entity_index = static_cast<int>(index);
        return result;
    }

    auto EntityId::for_ship(PlayerId player_id, EntityIndex index) -> EntityId {
        auto result = EntityId();
        result.type = EntityType::ShipEntity;
        result._player_id = player_id;
        result._entity_index = static_cast<int>(index);
        return result;
    }

    auto operator<<(std::ostream &ostream,
                    const EntityId &id) -> std::ostream & {
        switch (id.type) {
            case EntityType::InvalidEntity:ostream << "[Invalid ID]";
                break;
            case EntityType::PlanetEntity:
                ostream << "[Planet " << id.entity_index() << "]";
                break;
            case EntityType::ShipEntity:
                ostream << "[Ship " << static_cast<int>(id.player_id());
                ostream << ' ' << id.entity_index() << "]";
                break;
        }

        return ostream;
    }

    auto operator==(const EntityId &id1, const EntityId &id2) -> bool {
        return id1._player_id == id2._player_id
            && id1._entity_index == id2._entity_index;
    }

    auto operator!=(const EntityId &id1, const EntityId &id2) -> bool {
        return !(id1 == id2);
    }

    auto Ship::reset_docking_status() -> void {
        docking_status = DockingStatus::Undocked;
        docking_progress = 0;
        docked_planet = 0;
    }

    auto Ship::revive(const Location& loc) -> void {
        health = constants::BASE_SHIP_HEALTH;
        location = loc;
        weapon_cooldown = 0;
        radius = constants::SHIP_RADIUS;
        velocity = { 0, 0 };
        docking_status = DockingStatus::Undocked;
        docking_progress = 0;
        docked_planet = 0;
    }

    auto Planet::add_ship(EntityIndex ship) -> void {
        assert(docked_ships.size() < docking_spots);
        docked_ships.push_back(ship);
    }

    auto Planet::remove_ship(EntityIndex ship_id) -> void {
        auto pos = std::find(
            docked_ships.begin(),
            docked_ships.end(),
            ship_id
        );
        if (pos != docked_ships.end()) {
            docked_ships.erase(pos);
        }

        if (docked_ships.size() == 0) {
            owned = false;
            owner = 0;
        }
    }
}
