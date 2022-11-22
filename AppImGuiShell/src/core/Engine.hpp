#pragma once
#include "Window.hpp"
namespace AppEngine {
    class Engine {
        public:
            Engine();
            ~Engine();
        private:
            bool debugMode = true;
            Window* window{nullptr};

            
    };
}