#pragma once

// Delete Copy/Move Constructor
#define BOREALIS_DELETE_COPY_CONSTRUCT(type) \
    type(const type&) = delete;
#define BOREALIS_DELETE_MOVE_CONSTRUCT(type) \
    type(type&&) noexcept = delete;

// Delete Copy/Move Assignment
#define BOREALIS_DELETE_COPY_ASSIGN(type) \
    type& operator=(const type&) = delete;
#define BOREALIS_DELETE_MOVE_ASSIGN(type) \
    type& operator=(type&&) noexcept = delete;

// Default Copy/Move Constructor
#define BOREALIS_DEFAULT_COPY_CONSTRUCT(type) \
    type(const type&) = default;
#define BOREALIS_DEFAULT_MOVE_CONSTRUCT(type) \
    type(type&&) noexcept = default;

// Default Copy/Move Assignment
#define BOREALIS_DEFAULT_COPY_ASSIGN(type) \
    type& operator=(const type&) = default;
#define BOREALIS_DEFAULT_MOVE_ASSIGN(type) \
    type& operator=(type&&) noexcept = default;