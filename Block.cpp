#include "Block.hpp"

Block :: Block() {
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    selected = false;
    dragging = false;
    out1_selected = false;
    in1_selected = false;
    out2_selected = false;
    in2_selected = false;
}

Block :: ~Block() {

}

void Block :: setX(int nx) {
    x = nx;
}

int Block :: getX() {
    return x;
}

void Block :: setY(int ny) {
    y = ny;
}

int Block :: getY() {
    return y;
}

void Block :: setDragging(bool s) {
    dragging = s;
}

bool Block :: getDragging() {
    return dragging;
}

void Block :: setSelected(bool s) {
    selected = s;
}
bool Block :: getSelected() {
    return selected;
}

void Block :: setOut1Selected(bool s) {
    out1_selected = s;
}

bool Block :: getOut1Selected() {
    return out1_selected;
}

void Block :: setIn1Selected(bool s) {
    in1_selected = s;
}

bool Block :: getIn1Selected() {
    return in1_selected;
}

void Block :: setOut2Selected(bool s) {
    out2_selected = s;
}

bool Block :: getOut2Selected() {
    return out2_selected;
}

void Block :: setIn2Selected(bool s) {
    in2_selected = s;
}

bool Block :: getIn2Selected() {
    return in2_selected;
}

int Block :: getType() {
    return type;
}

void Block :: setTypeOfSensor(int t) {

}
int Block :: getTypeOfSensor() {
    return 0;
}

void Block :: setFunction(int f) {

}
int Block :: getFunction() {
    return 0;
}

void Block :: setWidth(int w) {
    width = w;
}

int Block :: getWidth() {
    return width;
}
void Block :: setHeight(int h) {
    height = h;
}
int Block :: getHeight() {
    return height;
}
/*
void Block :: setNext1(Block *b) {

}
Block Block :: getNext1() {
    return NULL;
}
void Block :: setNext2(Block *b) {

}
Block Block :: getNext2() {
    return NULL;
}
*/
