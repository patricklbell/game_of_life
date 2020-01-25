#include <SFML/Graphics.hpp>
#include <unordered_map> 
#include <array>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

// A hash function for arrays of ints
namespace std {
    template <> struct hash<std::array<int, 2>>
    {
        size_t operator()(const std::array<int, 2>& x) const
        {
            auto hash1 = std::hash<int>{}(x[0]);
            auto hash2 = std::hash<int>{}(x[1]);
            return hash1 ^ hash2;
        }
    };
}

int main()
{
    int moveSpeed = 2, sqrSize = 30;
    float updateGap = 0, updateTime = 0.2, zoom = 0.08;

    sf::RenderWindow app(sf::VideoMode(800, 600), "Conway's Game Of Life");
    app.setFramerateLimit(30);

    // we create our custom view
    sf::View user_view(sf::FloatRect(0, 0, app.getSize().x, app.getSize().y));

    sf::Clock clock;
    bool running = true, paused = false;
    sf::Event event;

    // Create board to be filled by config
    std::unordered_map<std::array<int, 2>, sf::RectangleShape> board; 

    // Load Config
    std::ifstream config("config.txt");
    std::string line;
    while (std::getline(config, line))
    {
        std::istringstream ss(line);
        std::vector<std::string> tokens{ std::istream_iterator<std::string>{ss},
            std::istream_iterator<std::string>{} };
        if (tokens.size() != 2) {
            std::cout << "Invalid Config" << std::endl;
            break;
        }
        int x = std::stoi(tokens[0]);
        int y = std::stoi(tokens[1]);
        sf::RectangleShape temp(sf::Vector2f(sqrSize, sqrSize));
        temp.setPosition(x * sqrSize, y * sqrSize);
        board[std::array<int, 2>{x, y}] = temp;
    }


    while (running)
    {
        while (app.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                running = false;
            
            if (event.type == sf::Event::Resized)
                user_view.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
        }
        float dt = clock.getElapsedTime().asSeconds();

        // moving user
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            user_view.move(0, -user_view.getSize().y / moveSpeed * dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            user_view.move(0, user_view.getSize().y / moveSpeed * dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            user_view.move(-user_view.getSize().x / moveSpeed * dt, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            user_view.move(user_view.getSize().x / moveSpeed * dt, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            user_view.zoom(1-zoom);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            user_view.zoom(1+zoom);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            running = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            paused = !paused;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            updateTime -= 0.1 * dt;
            std::cout << updateTime << std::endl;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            updateTime += 0.1 * dt;
            std::cout << updateTime << std::endl;
        }

        if (paused) {
            updateGap = 0;
        }
        else
        {
            updateGap += dt;
        }
        // Update squares
        if (updateGap > updateTime) {
            // Count number of neighbours for dead cells
            std::unordered_map<std::array<int, 2>, int> dead;
            std::vector<decltype(board)::key_type> deletes;
            int x, y;

            for (auto & it: board) {
                int x = it.first[0];
                int y = it.first[1];

                // Find neighbours
                int aliveNeighbours = 0;
                for (int i = -1; i < 2; i++){
                    for (int j = -1; j < 2; j++){
                        if (!(i == 0 && j == 0)) {
                            std::array<int, 2> temp{ x + i, y + j };
                            // See if neighbour is alive
                            if (board.count(temp)) {
                                aliveNeighbours++;
                            }
                            // Else add it to the list of dead
                            else
                            {
                                if (dead.count(temp)) {
                                    dead.at(temp)++;
                                }
                                else
                                {
                                    dead[temp] = 1;
                                }
                            }
                        }                        
                    }

                }
                
                // Does this cell survive?
                if (aliveNeighbours < 2 || aliveNeighbours > 3) {
                    deletes.emplace_back(it.first);
                    //std::cout << "cell " << x << ", " << y << " erased with " << aliveNeighbours << " count." << std::endl;
                }
            }
            // Delete dead cells
            for (auto&& key : deletes)
                board.erase(key);


            // Now handle the dead? cells
            for (auto& it: dead)
            {
                if (it.second == 3)
                {
                    sf::RectangleShape temp(sf::Vector2f(sqrSize, sqrSize));
                    temp.setPosition(it.first[0] * sqrSize, it.first[1] * sqrSize);
                    board[it.first] = temp;
                }
            }

            updateGap = 0;
        }

        clock.restart();
        app.setView(user_view);

        for (auto& it : board) {
            app.draw(it.second);
        }

        app.display();
        app.clear();
    }
    app.close();
    return 0;
}
