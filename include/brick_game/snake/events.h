#ifndef EVENTS_H
#define EVENTS_H

namespace brick_game {
enum class Event {
  ScorePoint,
  GameOver,
  NewLevel,
  TimeToMove,
  PlayerMoved,
  Paused,
  Unpaused,
  EventsSize
};

const std::size_t kEventsSize = static_cast<std::size_t>(Event::EventsSize);
constexpr int EventToInt(Event e) { return static_cast<int>(e); }
}  // namespace brick_game
#endif