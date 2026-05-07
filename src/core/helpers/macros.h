#pragma once

// Copy/Move Constructor
#define BOREALIS_DEFAULT_COPY_CONSTRUCT(type) \
    type(const type&) = delete;
#define BOREALIS_DEFAULT_MOVE_CONSTRUCT(type) \
    type(type&&) noexcept = delete;

// Copy/Move Assignment
#define BOREALIS_DEFAULT_COPY_ASSIGN(type) \
    type& operator=(const type&) = delete;
#define BOREALIS_DEFAULT_MOVE_ASSIGN(type) \
    type& operator=(type&&) noexcept = delete;