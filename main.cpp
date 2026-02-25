#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>

constexpr int TILE=10;

struct World{
    static constexpr int ROWS=70;
    static constexpr int COLS=100;
    std::vector<std::vector<int>> world{
        ROWS, std::vector<int>(COLS, 0)
    };

    /*FORMAS DE INICIALIZAR OBJETOS
     *std::vector<std::vector<int>> world{50, std::vector<int>(50,0)};
      std::vector<std::vector<int>> world = std::vector<std::vector<int>>(50, std::vector<int>(50,0));
      World(): world(50, std::vector<int>(50,0)){}
      World(){ world = std::vector<std::vector<int>>(50, std::vector<int>(50,0)); }

      Pero parentesis fuera de todas las funciones jamàs admite, tanto en class como en struct
      */

    int population=1500;

    int x, y;
    int row=world.size();
    int col=world[0].size();

    std::vector<int> _x={-1,0,1,-1,1,-1,0,1};
    std::vector<int> _y={-1,-1,-1,0,0,1,1,1};
    //std::vector<std::vector<int>> new_w=world;

    void draw(sf::RenderWindow& window){
        sf::RectangleShape cell(sf::Vector2f(TILE-1, TILE-1));
        cell.setFillColor(sf::Color::Cyan);

        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                if(world[i][j]!=0){
                    cell.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(cell);
                }
            }
        }
    }

    void run(){
        int _p=population;

        while(_p!=0){
            x=rand()%(row);
            y=rand()%(col);

            world[x][y]=1;

            _p--;
        }
    } 

    void elevate(){
        auto next=world;

        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                
                int neighbors=0;
                //int new_i, new_j;
                for(int k=0; k<8; k++){
                    int new_i=i+_x[k];
                    int new_j=j+_y[k];
                    if(new_i<0 || new_j<0 || new_i>=row || new_j>=col) continue;
                    neighbors+=world[new_i][new_j];
                }

                if(world[i][j]==0 && neighbors==3) next[i][j]=1; //NACE
                if(world[i][j]==1 && (neighbors>3 || neighbors<2)) next[i][j]=0; //MUERE
                //if(world[i][j]==1 && (neighbors==2 || neighbors==3)) continue;
            }
        }

        world=next;
    }
};

void execute(){
    World _w;
    _w.run();

    sf::RenderWindow window{
        sf::VideoMode({
                static_cast<unsigned>(_w.row*TILE),
                static_cast<unsigned>(_w.col*TILE)
        }), "TETRIS SFML!"
    };

    sf::Clock clock;

    while(window.isOpen()){
        while(const std::optional event=window.pollEvent()){
            if(event->is<sf::Event::Closed>()) window.close();
        }

        if(clock.getElapsedTime().asMilliseconds()>150){
            _w.elevate();
            clock.restart();
        }

        window.clear();
        _w.draw(window);
        window.display();
    }
}

int main(){

    execute();

    return 0;
}
