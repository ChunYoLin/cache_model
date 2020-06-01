#pragma once
#include <vector>
#include <cstdint>


class CacheLine
{
    private:
        uint32_t line_size;

    public:
        bool valid;
        uint32_t tag;
        std::vector<uint8_t> datas;

        CacheLine(uint32_t _line_size):
            line_size(_line_size)
        {
            datas.resize(_line_size);
        }
};

class CacheSet
{
    private:
        uint32_t line_size;
        uint32_t n_way;
        std::vector<CacheLine> lines;

    public:
        CacheSet(uint32_t _line_size, uint32_t _n_way):
            line_size(_line_size),
            n_way(_n_way),
            lines(std::vector<CacheLine>(_n_way, CacheLine(_line_size)))
        {
        }

        CacheLine* find_line(uint32_t tag)
        {
            for (auto& line : lines)
                if (line.valid && line.tag == tag)
                    return &line;
            return nullptr;
        }

        CacheLine* allocate_line(CacheLine line)
        {
            for (auto& _line : lines)
            {
                if (_line.valid)
                    continue;
                _line = line;
                return &_line;
            }
            lines[0] = line;
            return &lines[0];
        }

};

class Cache
{
    private:
        uint32_t size;
        uint32_t line_size;
        uint32_t n_way;
        std::vector<CacheSet> sets;

    public:
        struct Stat
        {
            uint32_t access = 0;
            uint32_t hit = 0;
        } stat;

        Cache(uint32_t _size, uint32_t _line_size, uint32_t _n_way):
            size(_size),
            line_size(_line_size),
            n_way(_n_way),
            sets(std::vector<CacheSet>(_size / (_line_size * _n_way), CacheSet(_line_size, _n_way)))
        {
        }

        CacheLine* find_line(uint32_t addr)
        {
            uint32_t idx = (addr / line_size) % sets.size();
            uint32_t tag = (addr / line_size) / sets.size();
            return sets[idx].find_line(tag);
        }

        CacheLine* allocate_line(uint32_t addr)
        {
            uint32_t idx = (addr / line_size) % sets.size();
            uint32_t tag = (addr / line_size) / sets.size();
            auto datas = read_emi(addr);
            auto line = CacheLine(line_size);
            line.valid = true;
            line.tag = tag;
            line.datas = datas;
            return sets[idx].allocate_line(line);
        }

        std::vector<uint8_t> read_emi(uint32_t addr)
        {
            return std::vector<uint8_t>(line_size, 0);
        }

        uint32_t read(uint32_t addr, uint32_t read_size)
        {
            stat.access++;
            uint32_t data = 0;
            uint32_t byte_offset = addr % (line_size - 1);
            for (uint32_t i = 0; i < read_size; ++i)
            {
                uint32_t _addr = addr + i;
                auto line = find_line(_addr);
                if (line == nullptr)
                    line = allocate_line(_addr);
                else
                    stat.hit++;
                uint8_t byte = line->datas[(byte_offset + i) % line_size];
                data |= byte <<= (i * 8);
            }
            return data;
        }

        void write(uint32_t addr, uint32_t data, uint32_t write_size)
        {
            stat.access++;
            uint32_t byte_offset = addr % (line_size - 1);
            for (uint32_t i = 0; i < write_size; ++i)
            {
                uint32_t _addr = addr + i;
                auto line = find_line(_addr);
                if (line == nullptr)
                    line = allocate_line(_addr);
                else
                    stat.hit++;
                line->datas[(byte_offset + i) % line_size] = (uint8_t)(data >> (i * 8));
            }
        }
};
