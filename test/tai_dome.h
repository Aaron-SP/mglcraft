/* Copyright [2013-2016] [Aaron Springstroh, Minimal Graphics Library]

This file is part of the MGLCraft.

MGLCraft is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MGLCraft is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MGLCraft.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __TEST_AI_TRAINER__
#define __TEST_AI_TRAINER__

#include <game/ai_trainer.h>
#include <game/cgrid.h>
#include <game/file.h>
#include <stdexcept>
#include <test.h>

bool test_ai_trainer()
{
    bool out = true;

    // Load the graph mesh with 128 pixel tile size
    game::cgrid grid(64, 8, 7);
    game::ai_trainer trainer;

    // Create start points
    std::vector<min::vec3<float>> start = {
        min::vec3<float>(-4.5, 30.5, 4.5),
        min::vec3<float>(-4.6, 31.5, 0.0),
        min::vec3<float>(-2.223, 32.5, -4.667),
        min::vec3<float>(2.0, 31.5, -4.5),
        min::vec3<float>(-4.5, 30.5, 0.0),
        min::vec3<float>(4.223, 32.5, 2.667),
        min::vec3<float>(4.5, 31.5, -2.0),
        min::vec3<float>(4.5, 30.5, 0.0),
        min::vec3<float>(4.5, 31.5, -4.5),
        min::vec3<float>(4.5, 31.5, 0.0),
        min::vec3<float>(0.0, 40.5, 0.0),
        min::vec3<float>(0.0, 25.5, 0.0)};

    // Create destination point
    std::vector<min::vec3<float>> dest = {
        min::vec3<float>(0.5, 36.0, -0.5),
        min::vec3<float>(21.0, 23.0, 0.0),
        min::vec3<float>(-21.0, 23.0, 0.0),
        min::vec3<float>(0.0, 23.0, 21.0),
        min::vec3<float>(0.0, 23.0, -21.0)};

    // Create output stream for loading AI
    std::vector<uint8_t> input;

    // Load data into stream from AI file
    game::load_file("data/ai/bot", input);
    if (input.size() != 0)
    {
        // load the data into the trainer of previous run
        trainer.deserialize(input);
    }

    // gradient based training
    float e0 = 100.0;
    float e1 = 100.0;
    for (size_t i = 0; i < 2000; i++)
    {
        std::cout << "iteration: " << i << std::endl;

        // Optimize net with back propagation
        e0 = e1;
        e1 = trainer.train_optimize(grid, start, dest);
        std::cout << "train_optimization error: " << e1 << std::endl;
        if (std::abs(e0 - e1) < 1E-4)
        {
            trainer.mutate_top();
        }
    }

    // Calculate top fitness of best network
    const float fitness = trainer.top_fitness(grid, start, dest);
    std::cout << "Top fitness is " << fitness << std::endl;

    // evolution based training
    for (size_t i = 0; i < 4; i++)
    {
        std::cout << "iteration: " << i << std::endl;

        // Solve
        for (size_t j = 0; j < 5; j++)
        {
            trainer.train_evolve(grid, start, dest);
        }

        // Mutate all nets
        trainer.mutate_pool();

        // Solve
        for (size_t j = 0; j < 5; j++)
        {
            trainer.train_evolve(grid, start, dest);
        }
    }

    // Calculate average and top fitness of all networks
    trainer.fitness(grid, start, dest);

    // Create output stream for saving bot
    std::vector<uint8_t> output;
    trainer.serialize(output);

    // Write data to file
    game::save_file("data/ai/bot", output);

    // return status
    return out;
}

#endif