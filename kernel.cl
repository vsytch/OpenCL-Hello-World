R"(
__kernel void hello_world()
{
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);
    size_t z = get_global_id(2);
}
)"