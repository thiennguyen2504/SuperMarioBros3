#include "VenusFire.h"
#include "Sprites.h"
#include "Mario.h"

VenusFire::VenusFire(float x, float y, int color,
    int spriteIdLeftDown, int spriteIdLeftUp, int spriteIdRightDown, int spriteIdRightUp)
    : Enemy(x, y)
{
    this->color = color;
    this->spriteIdLeftDown = spriteIdLeftDown;
    this->spriteIdLeftUp = spriteIdLeftUp;
    this->spriteIdRightDown = spriteIdRightDown;
    this->spriteIdRightUp = spriteIdRightUp;
    this->state = VENUS_FIRE_STATE_HIDDEN;
    this->moveTimer = 0;
    this->fireballTimer = 0;
    this->baseY = y; // Vị trí ban đầu (ẩn trong ống)
    this->targetY = y; // Mặc định mục tiêu là vị trí ban đầu
    this->isFacingRight = true; // Mặc định hướng phải
    this->isMarioAbove = false; // Mặc định Mario không ở trên
    this->vx = 0; // Khởi tạo vận tốc để tránh lỗi runtime
    this->vy = 0; // Khởi tạo vận tốc để tránh lỗi runtime
    this->ay = 0; // Tắt trọng lực vì VenusFire di chuyển thủ công
}

void VenusFire::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    // Sử dụng sprite "hướng lên" để tính hộp giới hạn (kích thước lớn nhất)
    CSprites* sprites = CSprites::GetInstance();
    LPSPRITE sprite = sprites->Get(spriteIdLeftUp); // Dùng sprite "left up" để tính kích thước
    if (sprite)
    {
        left = x - (sprite->GetRight() - sprite->GetLeft() + 1) / 2;
        top = y - (sprite->GetBottom() - sprite->GetTop() + 1) / 2;
        right = left + (sprite->GetRight() - sprite->GetLeft() + 1);
        bottom = top + (sprite->GetBottom() - sprite->GetTop() + 1);
    }
    else
    {
        left = x - 8;  // Giá trị mặc định nếu sprite không tồn tại
        top = y - 16;
        right = x + 8;
        bottom = y + 16;
    }
}

void VenusFire::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    // Tìm Mario để kiểm tra khoảng cách và xác định hướng
    CMario* mario = nullptr;
    for (size_t i = 0; i < coObjects->size(); i++)
    {
        if (dynamic_cast<CMario*>(coObjects->at(i)))
        {
            mario = dynamic_cast<CMario*>(coObjects->at(i));
            break;
        }
    }

    bool isMarioNear = false;
    if (mario)
    {
        float mx, my;
        mario->GetPosition(mx, my);
        float dx = abs(x - mx);
        float dy = abs(y - my);
        float distance = sqrt(dx * dx + dy * dy);
        isMarioNear = (distance < VENUS_FIRE_ATTACK_RANGE);

        // Cập nhật hướng dựa trên vị trí của Mario
        isFacingRight = (mx > x);
        isMarioAbove = (my < y);
    }

    // Quản lý trạng thái và chuyển động
    moveTimer += dt;
    fireballTimer += dt;

    // Xác định vị trí mục tiêu dựa trên trạng thái
    CSprites* sprites = CSprites::GetInstance();
    LPSPRITE sprite = sprites->Get(spriteIdLeftUp);
    float spriteHeight = sprite ? (sprite->GetBottom() - sprite->GetTop() + 1) : 32;
    float maxY = baseY - spriteHeight; // Vị trí cao nhất khi thò ra

    if (!isMarioNear)
    {
        // Nếu Mario không ở gần, ẩn trong ống
        SetState(VENUS_FIRE_STATE_HIDDEN);
    }
    else
    {
        // Mario ở gần, thực hiện chu kỳ thò ra, bắn, rút vào
        if (state == VENUS_FIRE_STATE_HIDDEN)
        {
            SetState(VENUS_FIRE_STATE_EMERGING);
        }
        else if (state == VENUS_FIRE_STATE_EMERGING && moveTimer >= VENUS_FIRE_EMERGE_TIME)
        {
            SetState(VENUS_FIRE_STATE_ATTACKING);
        }
        else if (state == VENUS_FIRE_STATE_ATTACKING && moveTimer >= VENUS_FIRE_ATTACK_TIME)
        {
            SetState(VENUS_FIRE_STATE_RETREATING);
        }
        else if (state == VENUS_FIRE_STATE_RETREATING && moveTimer >= VENUS_FIRE_RETREAT_TIME)
        {
            SetState(VENUS_FIRE_STATE_HIDDEN);
        }
    }

    // Cập nhật vị trí mục tiêu
    if (state == VENUS_FIRE_STATE_EMERGING || state == VENUS_FIRE_STATE_ATTACKING)
    {
        targetY = maxY; // Di chuyển lên
    }
    else if (state == VENUS_FIRE_STATE_RETREATING || state == VENUS_FIRE_STATE_HIDDEN)
    {
        targetY = baseY; // Di chuyển xuống
    }

    // Di chuyển từ từ đến targetY bằng vận tốc vy
    float distanceToTarget = targetY - y;
    float moveTime = (state == VENUS_FIRE_STATE_EMERGING || state == VENUS_FIRE_STATE_RETREATING) ?
        (state == VENUS_FIRE_STATE_EMERGING ? VENUS_FIRE_EMERGE_TIME : VENUS_FIRE_RETREAT_TIME) : 1;
    vy = distanceToTarget / moveTime; // Tính vận tốc dựa trên thời gian di chuyển

    // Gọi Update của Enemy để xử lý logic chung (như va chạm), nhưng không để Enemy thay đổi vy
    Enemy::Update(dt, coObjects);

    // Ghi đè lại vy để đảm bảo di chuyển từ từ của VenusFire
    y += vy * dt; // Di chuyển từ từ

    // Giới hạn vị trí y
    if (state == VENUS_FIRE_STATE_EMERGING && y < maxY) y = maxY;
    if (state == VENUS_FIRE_STATE_RETREATING && y > baseY) y = baseY;

    // Bắn cầu lửa khi ở trạng thái tấn công
    if (state == VENUS_FIRE_STATE_ATTACKING && fireballTimer >= VENUS_FIRE_FIREBALL_COOLDOWN)
    {
        // TODO: Tạo Fireball tại vị trí x, y với hướng dựa trên vị trí Mario
        fireballTimer = 0;
    }
}

void VenusFire::Render()
{
    CSprites* sprites = CSprites::GetInstance();

    int spriteId;
    if (isMarioAbove)
    {
        spriteId = isFacingRight ? spriteIdRightUp : spriteIdLeftUp; // Hướng lên
    }
    else
    {
        spriteId = isFacingRight ? spriteIdRightDown : spriteIdLeftDown; // Hướng xuống
    }

    sprites->Get(spriteId)->Draw(x, y);
    RenderBoundingBox();
}

void VenusFire::OnCollisionWith(LPCOLLISIONEVENT e)
{
    // Không cho phép tiêu diệt bằng cách nhảy lên đầu (ny < 0: va chạm từ trên xuống)
    if (e->ny < 0) return;

    // Các va chạm khác (nếu có) sẽ được xử lý sau khi có Fireball
    Enemy::OnCollisionWith(e);
}

void VenusFire::SetState(int state)
{
    Enemy::SetState(state);
    this->state = state;
    moveTimer = 0; // Reset timer khi chuyển trạng thái
    vy = 0; // Reset vận tốc khi chuyển trạng thái
}