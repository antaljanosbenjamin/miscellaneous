#include "MouseStateHandler.hpp"
#include <unordered_map>
#include "utils/HashCombine.hpp"

namespace minesweeper_gui {

bool operator==(const MouseStateHandler::State &lhs, const MouseStateHandler::State &rhs) {
  return lhs.buttonState == rhs.buttonState && lhs.isActive == rhs.isActive;
};

namespace {
using State = MouseStateHandler::State;
using ButtonsState = MouseStateHandler::ButtonsState;
using Event = MouseStateHandler::Event;
using Action = MouseStateHandler::Action;

constexpr auto active = true;
constexpr auto inactive = true;

struct TransitionResult {
  State newState;
  Action action;
};

struct StateHasher {
  size_t operator()(const State &state) const {
    size_t result{0};
    utils::hashCombine(result, state.buttonState);
    utils::hashCombine(result, state.isActive);
    return result;
  }
};

const std::unordered_map<State, std::unordered_map<Event, TransitionResult>, StateHasher> stateTransition{
    {
        {ButtonsState::Default, active},
        {
            {Event::LeftDown, {{ButtonsState::LeftIsDown, active}, Action::Hover}},
            {Event::RightDown, {{ButtonsState::RightIsDown, active}, Action::Flag}},
            {Event::MiddleDown, {{ButtonsState::RightIsDown, active}, Action::HoverNeighbors}},
            {Event::Leave, {{ButtonsState::Default, active}, Action::None}},
            {Event::Enter, {{ButtonsState::Default, active}, Action::None}},
        },
    },
    {
        {ButtonsState::LeftIsDown, active},
        {
            {Event::RightDown, {{ButtonsState::LeftAndRightAreDown, active}, Action::HoverNeighbors}},
            {Event::MiddleDown, {{ButtonsState::LeftAndMiddleAreDown, active}, Action::HoverNeighbors}},
            {Event::LeftUp, {{ButtonsState::Default, active}, Action::Open}},
            {Event::Leave, {{ButtonsState::LeftIsDown, active}, Action::None}},
            {Event::Enter, {{ButtonsState::LeftIsDown, active}, Action::None}},
        },
    },
    {
        {ButtonsState::RightIsDown, active},
        {
            {Event::LeftDown, {{ButtonsState::LeftAndRightAreDown, active}, Action::HoverNeighbors}},
            {Event::MiddleDown, {{ButtonsState::RightAndMiddleAreDown, active}, Action::HoverNeighbors}},
            {Event::RightUp, {{ButtonsState::Default, active}, Action::Flag}},
            {Event::Leave, {{ButtonsState::RightIsDown, active}, Action::None}},
            {Event::Enter, {{ButtonsState::RightIsDown, active}, Action::None}},
        },
    },
    {
        {ButtonsState::MiddleIsDown, active},
        {
            {Event::LeftDown, {{ButtonsState::LeftAndMiddleAreDown, active}, Action::None}},
            {Event::RightDown, {{ButtonsState::RightAndMiddleAreDown, active}, Action::None}},
            {Event::MiddleUp, {{ButtonsState::Default, active}, Action::OpenNeighbors}},
            {Event::Leave, {{ButtonsState::MiddleIsDown, active}, Action::None}},
            {Event::Enter, {{ButtonsState::MiddleIsDown, active}, Action::None}},
        },
    },
    {
        {ButtonsState::LeftAndRightAreDown, active},
        {
            {Event::MiddleDown, {{ButtonsState::AllAreDown, active}, Action::None}},
            {Event::LeftUp, {{ButtonsState::RightIsDown, inactive}, Action::OpenNeighbors}},
            {Event::RightUp, {{ButtonsState::LeftIsDown, inactive}, Action::OpenNeighbors}},
            {Event::Leave, {{ButtonsState::LeftAndRightAreDown, active}, Action::None}},
            {Event::Enter, {{ButtonsState::LeftAndRightAreDown, active}, Action::None}},
        },
    },
    {
        {ButtonsState::LeftAndMiddleAreDown, active},
        {
            {Event::RightDown, {{ButtonsState::AllAreDown, active}, Action::None}},
            {Event::MiddleUp, {{ButtonsState::LeftIsDown, inactive}, Action::OpenNeighbors}},
            {Event::LeftUp, {{ButtonsState::MiddleIsDown, inactive}, Action::OpenNeighbors}},
            {Event::Leave, {{ButtonsState::LeftAndMiddleAreDown, active}, Action::None}},
            {Event::Enter, {{ButtonsState::LeftAndMiddleAreDown, active}, Action::None}},
        },
    },
    {
        {ButtonsState::RightAndMiddleAreDown, active},
        {
            {Event::LeftDown, {{ButtonsState::AllAreDown, active}, Action::None}},
            {Event::RightUp, {{ButtonsState::MiddleIsDown, inactive}, Action::OpenNeighbors}},
            {Event::MiddleUp, {{ButtonsState::RightIsDown, inactive}, Action::OpenNeighbors}},
            {Event::Leave, {{ButtonsState::RightAndMiddleAreDown, active}, Action::None}},
            {Event::Enter, {{ButtonsState::RightAndMiddleAreDown, active}, Action::None}},
        },
    },
    {
        {ButtonsState::AllAreDown, active},
        {
            {Event::LeftUp, {{ButtonsState::RightAndMiddleAreDown, inactive}, Action::OpenNeighbors}},
            {Event::RightUp, {{ButtonsState::LeftAndMiddleAreDown, inactive}, Action::OpenNeighbors}},
            {Event::MiddleUp, {{ButtonsState::LeftAndRightAreDown, inactive}, Action::OpenNeighbors}},
            {Event::Leave, {{ButtonsState::RightAndMiddleAreDown, active}, Action::None}},
            {Event::Enter, {{ButtonsState::RightAndMiddleAreDown, active}, Action::None}},
        },
    },
    // Inactive states
    {
        {ButtonsState::LeftIsDown, inactive},
        {
            {Event::RightDown, {{ButtonsState::LeftAndRightAreDown, active}, Action::None}},
            {Event::MiddleDown, {{ButtonsState::LeftAndMiddleAreDown, active}, Action::None}},
            {Event::LeftUp, {{ButtonsState::Default, inactive}, Action::None}},
            {Event::Leave, {{ButtonsState::LeftIsDown, inactive}, Action::None}},
            {Event::Enter, {{ButtonsState::LeftIsDown, inactive}, Action::None}},
        },
    },
    {
        {ButtonsState::RightIsDown, inactive},
        {
            {Event::LeftDown, {{ButtonsState::LeftAndRightAreDown, active}, Action::None}},
            {Event::MiddleDown, {{ButtonsState::RightAndMiddleAreDown, active}, Action::None}},
            {Event::RightUp, {{ButtonsState::Default, inactive}, Action::None}},
            {Event::Leave, {{ButtonsState::RightIsDown, inactive}, Action::None}},
            {Event::Enter, {{ButtonsState::RightIsDown, inactive}, Action::None}},
        },
    },
    {
        {ButtonsState::MiddleIsDown, inactive},
        {
            {Event::LeftDown, {{ButtonsState::LeftAndMiddleAreDown, active}, Action::None}},
            {Event::RightDown, {{ButtonsState::RightAndMiddleAreDown, active}, Action::None}},
            {Event::MiddleUp, {{ButtonsState::Default, inactive}, Action::None}},
            {Event::Leave, {{ButtonsState::MiddleIsDown, inactive}, Action::None}},
            {Event::Enter, {{ButtonsState::MiddleIsDown, inactive}, Action::None}},
        },
    },
    {
        {ButtonsState::LeftAndRightAreDown, inactive},
        {
            {Event::MiddleDown, {{ButtonsState::AllAreDown, active}, Action::None}},
            {Event::LeftUp, {{ButtonsState::RightIsDown, inactive}, Action::None}},
            {Event::RightUp, {{ButtonsState::LeftIsDown, inactive}, Action::None}},
            {Event::Leave, {{ButtonsState::LeftAndRightAreDown, inactive}, Action::None}},
            {Event::Enter, {{ButtonsState::LeftAndRightAreDown, inactive}, Action::None}},
        },
    },
    {
        {ButtonsState::LeftAndMiddleAreDown, inactive},
        {
            {Event::RightDown, {{ButtonsState::AllAreDown, active}, Action::None}},
            {Event::MiddleUp, {{ButtonsState::LeftIsDown, inactive}, Action::None}},
            {Event::LeftUp, {{ButtonsState::MiddleIsDown, inactive}, Action::None}},
            {Event::Leave, {{ButtonsState::LeftAndMiddleAreDown, inactive}, Action::None}},
            {Event::Enter, {{ButtonsState::LeftAndMiddleAreDown, inactive}, Action::None}},
        },
    },
    {
        {ButtonsState::RightAndMiddleAreDown, inactive},
        {
            {Event::LeftDown, {{ButtonsState::AllAreDown, active}, Action::None}},
            {Event::RightUp, {{ButtonsState::MiddleIsDown, inactive}, Action::None}},
            {Event::MiddleUp, {{ButtonsState::RightIsDown, inactive}, Action::None}},
            {Event::Leave, {{ButtonsState::RightAndMiddleAreDown, inactive}, Action::None}},
            {Event::Enter, {{ButtonsState::RightAndMiddleAreDown, inactive}, Action::None}},
        },
    },
};

} // namespace

Action MouseStateHandler::leave() {
  return Action::None;
}

Action MouseStateHandler::leftDown() {
  return Action::None;
}

Action MouseStateHandler::leftUp() {
  return Action::None;
}

Action MouseStateHandler::rightDown() {
  return Action::None;
}

Action MouseStateHandler::rightUp() {
  return Action::None;
}

Action MouseStateHandler::middleDown() {
  return Action::None;
}

Action MouseStateHandler::middleUp() {
  return Action::None;
}
} // namespace minesweeper_gui