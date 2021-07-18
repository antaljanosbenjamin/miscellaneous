#pragma once

namespace minesweeper_gui {

enum class LeftState {
  Up,
  Down,
};

enum class RightState {
  Up,
  Down,
};

enum class MiddleState {
  Up,
  Down,
};

class MouseStateHandler {

public:
  enum class ButtonsState {
    Default,
    LeftIsDown,
    RightIsDown,
    MiddleIsDown,
    LeftAndRightAreDown,
    LeftAndMiddleAreDown,
    RightAndMiddleAreDown,
    AllAreDown,
  };

  enum class Event {
    LeftDown,
    LeftUp,
    RightDown,
    RightUp,
    MiddleDown,
    MiddleUp,
    Leave,
    Enter,
  };

  enum class Action {
    Hover,
    Unhover,
    Open,
    HoverNeighbors,
    UnhoverNeighbors,
    OpenNeighbors,
    Flag,
    None,
  };

  struct State {
    ButtonsState buttonState{ButtonsState::Default};
    bool isActive{true};
  };

  // Action enter(const LeftState left, const RightState right, const MiddleState middle) {
  //   state = [&]() {
  //     if (middle == MiddleState::Down || (left == LeftState::Down && right == RightState::Down)) {
  //       return State::MiddleIsDown;
  //     }
  //     if (left == LeftState::Down) {
  //       return State::LeftIsDown;
  //     }
  //     if (right == RightState::Down) {
  //       return State::RightIsDown;
  //     }
  //   }();
  //   return Action::None;
  // }

  Action leave();

  Action leftDown();

  Action leftUp();

  Action rightDown();

  Action rightUp();

  Action middleDown();

  Action middleUp();

private:
  State state{};
};
} // namespace minesweeper_gui
