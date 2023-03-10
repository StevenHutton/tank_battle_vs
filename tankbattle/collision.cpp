#include "collision.h"

rect GetEntityRect(Entity ent)
{
	rect r;

	f32 half_width = ent.width / 2.0f;
	f32 half_height = ent.height / 2.0f;

	r.left = ent.pos.x - half_width;
	r.right = ent.pos.x + half_width;
	r.top = ent.pos.y + half_height;
	r.bottom = ent.pos.y - half_height;

	return r;
}

rect GetExpandedRect(Entity ent, f32 halfWidth, f32 halfHeight, f32 modifier)
{
	rect r = GetEntityRect(ent);

	r.left = r.left - halfWidth - modifier;
	r.right = r.right + halfWidth + modifier;
	r.top = r.top + halfHeight + modifier;
	r.bottom = r.bottom - halfHeight - modifier;

	return r;
}

#define COLLISION_EPSILON 0.00001f

bool Is_Penetration(Entity e1, Entity e2, Penetration& pen)
{
	if (!e2.is_active) {
		return false;
	}

	f32 offset_x = e1.pos.x - e2.pos.x;
	f32 offset_y = e1.pos.y - e2.pos.y;

	f32 offset_mag_x = abs(offset_x);
	f32 offset_mag_y = abs(offset_y);

	f32 combined_half_width = (e1.width / 2.f) + (e2.width / 2.f);
	f32 combined_half_height = (e1.height / 2.f) + (e2.height / 2.f);

	if (((combined_half_width - offset_mag_x) >= COLLISION_EPSILON) &&
		((combined_half_height - offset_mag_y) >= COLLISION_EPSILON))
	{
		pen.depth.x = combined_half_width - offset_mag_x;
		pen.depth.y = combined_half_height - offset_mag_y;

		//move apart in the axis of least penetration
		if (pen.depth.x <= pen.depth.y)
		{
			if (offset_x > 0)
				pen.normal = { 1, 0 };
			else
				pen.normal = { -1, 0 };
		}
		else
		{
			if (offset_y > 0)
				pen.normal = { 0, 1 };
			else
				pen.normal = { 0, -1 };
		}

		return true;
	}
	return false;
}

int get_worst_pen_index(Penetration pens[], int pen_count)
{
	float pen_depth = 0.0f;
	int index = -1;
	for (int i = 0; i < pen_count; i++)
	{
		if (pens[i].depth.x >= pen_depth || pens[i].depth.y >= pen_depth)
		{
			pen_depth = maxf(pens[i].depth.x, pens[i].depth.y);
			index = i;
		}
	}

	return index;
}

bool Is_Collision(Entity e1, Entity e2, Collision& col, f32 dt)
{
	if (!e2.is_active) {
		return false;
	}

	rect r = GetExpandedRect(e2, e1.width / 2, e1.height / 2, -0.01f);

	//check for obvious misses
	if (e1.velocity.x == 0.0f &&
		(e1.pos.x <= r.left || e1.pos.x >= r.right))
		return false;

	if (e1.velocity.y == 0.0f &&
		(e1.pos.y <= r.bottom || e1.pos.y >= r.top))
		return false;

	f32 left_offset = (r.left - e1.pos.x);
	f32 right_offset = (r.right - e1.pos.x);
	f32 top_offset = (r.top - e1.pos.y);
	f32 bottom_offset = (r.bottom - e1.pos.y);

	f32 leftIntercept = left_offset / (e1.velocity.x * dt);
	f32 rightIntercept = right_offset / (e1.velocity.x * dt);
	f32 topIntercept = top_offset / (e1.velocity.y * dt);
	f32 bottomIntercept = bottom_offset / (e1.velocity.y * dt);

	f32 x1, x2, y1, y2;

	if (leftIntercept < rightIntercept)
	{
		x1 = leftIntercept;
		x2 = rightIntercept;
	}
	else
	{
		x2 = leftIntercept;
		x1 = rightIntercept;
	}

	if (topIntercept < bottomIntercept)
	{
		y1 = topIntercept;
		y2 = bottomIntercept;
	}
	else
	{
		y2 = topIntercept;
		y1 = bottomIntercept;
	}

	if (x1 > y2 || y1 > x2) return false;

	f32 c1 = maxf(x1, y1);
	if (c1 < 0.0f || c1 >= 1.0f) return false;
	f32 c2 = minf(x2, y2);
	if (c2 < 0) return false;

	col.time = c1;

	if (x1 > y1)
	{
		if (e1.velocity.x < 0)
			col.normal = { 1, 0 };
		else
			col.normal = { -1, 0 };
	}
	else if (x1 < y1)
	{
		if (e1.velocity.y < 0)
			col.normal = { 0, 1 };
		else
			col.normal = { 0, -1 };
	}

	return true;
}

void resolve_swept_collisions_with_terrain(Entity* ent, Collision cols[], int num_cols)
{
	for (int i = 0; i < num_cols; i++)
	{
		Collision col = cols[i];

		ent->velocity.x += abs(ent->velocity.x)
			* col.normal.x * (1 - col.time);
		ent->velocity.y += abs(ent->velocity.y)
			* col.normal.y * (1 - col.time);
	}
}

bool Is_Penetration_Naive(Entity e1, Entity e2)
{
	rect r1 = GetExpandedRect(e1, e2.width / 2.f, e2.height / 2.f);

	if (e2.pos.x > r1.left && e2.pos.x < r1.right &&
		e2.pos.y > r1.bottom && e2.pos.y < r1.top)
	{
		return true;
	}
	return false;
}

bool is_point_collision(Entity ent, Vector2 pos)
{
	rect r1 = GetEntityRect(ent);

	if (pos.x > r1.left && pos.x < r1.right &&
		pos.y > r1.bottom && pos.y < r1.top)
	{
		return true;
	}
	return false;
}