

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:28:12 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/07 22:41:27 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <MLX42/MLX42.h>
#include <stdint.h>
#include "collection/ft_arraylist.h"
#include "ft_memlib.h"
#include "get_next_line.h"
#include "scratch.h"
#include "ft_stdio.h"
#include "ft_util.h"
#include "ft_string.h"
#include "ft_ctype.h"

int32_t system_invalid(char *err_msg)
{
	ft_putstr_fd("ERROR:\n\t", 2);	
	ft_putstr_fd(err_msg, 2);
	ft_putstr_fd("\n", 2);
	return (0);
}

int32_t	system_panic(t_game *game, t_exit_status exit_code, char *msg)
{
	game->exit_status = exit_code;
	if (game->mlx)
		mlx_close_window(game->mlx);
	if (exit_code == MLX_ERROR)
		ft_putstr_fd((char *) mlx_strerror(mlx_errno), 2);
	else if (msg)
	{
		ft_putstr_fd("ERROR:\n\t", 2);	
		ft_putstr_fd(msg, 2);
		ft_putstr_fd("\n", 2);
	}
	return (exit_code);
}

void	system_quit_ok(void *param)
{
	t_game	*game;

	game = param;
	game->exit_status = OK;
	if (game->mlx)
		mlx_close_window(game->mlx);
}

void	entity_hero_animate(t_entity *hero)
{
	t_drawable	*dwble;
	size_t		i;
	size_t		len;

	i = 0;
	len = ft_arraylist_len(hero->drawables);
	while (i < len)
	{
		dwble = ft_arraylist_get(hero->drawables, i);
		dwble->img->instances[dwble->i].enabled = (
				(size_t) mlx_get_time()) % 2 == i;
		i++;
	}
}

t_moveable	*entity_hero_get_moveable(t_entity *hero)
{
	t_component	*component;

	component = ft_arraylist_get(hero->components, 0);
	return (component->cmp);
}

t_entity	*map_get_entity(t_game *game, int x, int y)
{
	return (ft_arraylist_get(ft_arraylist_get(game->map.chart, y), x));
}

void	system_hero_wait_input(t_game *game, t_entity *hero)
{
	t_direction	direction;
	t_moveable	*moveable;

	if (game->state.acc_time < 0.02 )
	{
		game->state.acc_time += game->mlx->delta_time;
	}
	else
	{
		moveable = entity_hero_get_moveable(hero);
		direction = moveable->move(hero, game);
		if (direction != IDLE)
		{
			game->state.gst = HERO_MOVE;
			game->state.acc_time = 0;
			moveable->direction = direction;
		}
	}
}

void	drawables_update_position(
	t_arraylist drawables, int x, int y, t_context ctx)
{
	size_t		i;
	size_t		len;
	t_drawable	*dwble;

	i = 0;
	len = ft_arraylist_len(drawables);
	while (i < len)
	{
		dwble = ft_arraylist_get(drawables, i);
		dwble->img->instances[dwble->i].x = (
			x * ctx.block_size + ctx.window_x_offset + ctx.overflow_x_offset);
		dwble->img->instances[dwble->i].y = (
			y * ctx.block_size + ctx.window_y_offset + ctx.overflow_y_offset);
		i++;
	}
}

void	system_hero_update_pos(t_game *game)
{
	drawables_update_position(
		game->map.hero->drawables,
		game->map.hero->x,
		game->map.hero->y,
		game->ctx);
}


void map_update_entity_pos(t_entity *entity, t_drawable *dwb, t_game *game)
{
	if (!dwb)
		return ;
	dwb->img->instances[dwb->i].x = game->ctx.overflow_x_offset +
		(game->ctx.block_size * entity->x) + game->ctx.window_x_offset;
	dwb->img->instances[dwb->i].y = game->ctx.overflow_y_offset +
		(game->ctx.block_size * entity->y) + game->ctx.window_y_offset;
}

void	map_hero_collect_item(
	t_game *game, t_entity *item, size_t hero_pos[2], size_t item_pos[2])
{
	t_entity	*hero;
	t_drawable	*itm_dwble;

	hero = game->map.hero;
	itm_dwble = ft_arraylist_get(item->drawables, 0);
	itm_dwble->img->instances[itm_dwble->i].enabled = false;
	ft_arraylist_replace2d(game->map.chart, hero, item_pos[0], item_pos[1]);
	ft_arraylist_switch2d(game->map.chart, NULL, hero_pos[0], hero_pos[1]);
	game->state.collectables_count--;
	if (game->state.collectables_count == 0)
	{
		game->state.gst = EXIT_ENABLE;
		system_hero_update_pos(game);
	}
}

void map_hero_move_to_floor(
	t_game *game, size_t new_pos[2], size_t old_pos[2])
{
	ft_arraylist_swap2d(game->map.chart, new_pos, old_pos);
}

void	map_hero_move_to_exit(
	t_game *game)
{
	game->state.gst = GAME_END;
}

void	map_hero_switch_position(
	t_game *game, t_entity *other, size_t new_pos[2], size_t old_pos[2])
{	
	if (!other)
		map_hero_move_to_floor(game, new_pos, old_pos);
	else if (other->type == ITEM)
		map_hero_collect_item(game, other, old_pos, new_pos);
	else if (other->type == EXIT)
		map_hero_move_to_exit(game);
}

// pos[y, x]
void	system_hero_move(t_game *game, t_entity *hero)
{
	size_t		new_pos[2];
	size_t		old_pos[2];
	int32_t		test_pos[2];
	t_moveable	*m;
	t_entity	*other;

	m = entity_hero_get_moveable(hero);
	new_pos[1] = hero->x + (m->direction == RIGHT) - (m->direction == LEFT);
	new_pos[0] = hero->y + (m->direction == DOWN) - (m->direction == UP);
	m->direction = IDLE;
	other = map_get_entity(game, new_pos[1], new_pos[0]);
	mlx_get_window_pos(game->mlx, test_pos + 1, test_pos);
	if (!other || other->type == ITEM || other->type == EXIT)
	{
		old_pos[1] = hero->x;
		old_pos[0] = hero->y;
		map_hero_switch_position(game, other, new_pos, old_pos);
		hero->x = (int) new_pos[1];
		hero->y = (int) new_pos[0];
		game->state.move_count++;
		ft_printf("movement count: %5d\n", game->state.move_count);
	}
	if (game->state.gst == HERO_MOVE)
		game->state.gst = HERO_UPDATE_POS;
}

void	system_exit_enable(t_game *game)
{
	t_entity	*exit;
	t_drawable	*drawable;

	exit = game->map.exit;
	ft_arraylist_switch2d(game->map.chart, exit, exit->y, exit->x);
	drawable = ft_arraylist_get(exit->drawables, 0);
	drawable->img->instances[drawable->i].enabled = 1;
	map_update_entity_pos(exit, drawable, game);
	game->state.gst = HERO_UPDATE_POS;
}

void	system_game_end(t_game *game)
{
	t_drawable		*exit_drawable;
	int				is_enabled;

	exit_drawable = ft_arraylist_get(game->map.exit->drawables, 0);
	is_enabled = ((int) mlx_get_time()) % 3 == 0;
	exit_drawable->img->instances[exit_drawable->i].enabled = is_enabled;
}

t_screen_overflow	system_hero_screen_overflown(t_game *game)
{
	t_drawable			*dwb;
	mlx_instance_t		ist;
	t_screen_overflow	over;
	int					 half_block;

	over = 0;
	dwb = ft_arraylist_get(game->map.hero->drawables, 0);
	ist = dwb->img->instances[0];
	half_block = game->ctx.block_size / 2;
	if (ist.x + half_block < 0)
		over |= SCREEN_OVERFLOW_LEFT;
	else if (ist.x + half_block > game->mlx->width)
		over |= SCREEN_OVERFLOW_RIGHT;
	if (ist.y + half_block < 0)
		over |= SCREEN_OVERFLOW_UP;
	else if (ist.y + half_block > game->mlx->height)
		over |= SCREEN_OVERFLOW_DOWN;
	return (over);
}

void system_map_update_all_drawables_pos(t_game *game)
{
	size_t		chart_len[2];
	size_t		i[3];
	t_arraylist map_row;
	t_entity   *entity;
	size_t		drawables_len;
	
	chart_len[0] = ft_arraylist_len(game->map.chart);
	i[0] = 0;
	while (i[0] < chart_len[0])
	{
		map_row = ft_arraylist_get(game->map.chart, i[0]);
		chart_len[1] = ft_arraylist_len(map_row);
		i[1] = -1;
		while (++i[1] < chart_len[1])
		{
			entity = ft_arraylist_get(map_row, i[1]);
			if (!entity || !entity->drawables)
				continue ;
			drawables_len = ft_arraylist_len(entity->drawables);
			i[2] = -1;
			while (++i[2] < drawables_len)
				map_update_entity_pos(
					entity, ft_arraylist_get(entity->drawables, i[2]), game);
		}
		i[0]++;
	}
}

void system_map_update_pos(t_screen_overflow over, t_game *game)
{
	if ((over & SCREEN_OVERFLOW_UP) == SCREEN_OVERFLOW_UP)
		game->ctx.overflow_y_offset += game->mlx->height; 
	else if ((over & SCREEN_OVERFLOW_DOWN) == SCREEN_OVERFLOW_DOWN)
		game->ctx.overflow_y_offset -= game->mlx->height; 
	else if ((over & SCREEN_OVERFLOW_LEFT) == SCREEN_OVERFLOW_LEFT)
		game->ctx.overflow_x_offset += game->mlx->width; 
	else if ((over & SCREEN_OVERFLOW_RIGHT) == SCREEN_OVERFLOW_RIGHT)
		game->ctx.overflow_x_offset -= game->mlx->width;
	system_map_update_all_drawables_pos(game);
	game->state.gst = HERO_WAIT;
}

void	system_loop(t_game *game)
{
	if (mlx_is_key_down(game->mlx, MLX_KEY_ESCAPE))
	{
		game->exit_status = OK;
		mlx_close_window(game->mlx);
	}
	entity_hero_animate(game->map.hero);
	if (game->state.gst == HERO_WAIT)
		system_hero_wait_input(game, game->map.hero);
	else if (game->state.gst == HERO_MOVE)
		system_hero_move(game, game->map.hero);
	else if (game->state.gst == HERO_UPDATE_POS)
	{
		system_hero_update_pos(game);
		if (system_hero_screen_overflown(game) > 0)
			game->state.gst = MAP_UPDATE_POS;
		else 
			game->state.gst = HERO_WAIT;
	}
	else if (game->state.gst == MAP_UPDATE_POS)
		system_map_update_pos(system_hero_screen_overflown(game), game);
	else if (game->state.gst == EXIT_ENABLE)	
		system_exit_enable(game);
	else if (game->state.gst == GAME_END)
	{
		system_hero_update_pos(game);
		system_game_end(game);
	}
}

int32_t	context_load_asset(
	t_context *ctx, char *path, mlx_t *mlx, t_entity_type type)
{
	t_drawable		*drawable;
	mlx_texture_t	*texture;
	t_arraylist		lst;

	drawable = malloc(sizeof(t_drawable));
	if (!drawable)
		return (system_invalid("Failed to allocate memory for drawable"));
	lst = ft_arraylist_get(ctx->drawables, type);
	if (!lst)
		return (system_invalid("List of drawables wall null for some type"));
	lst = ft_arraylist_add(lst, drawable);
	if (!lst)
		return (system_invalid("List was null after adding drawable"));
	texture = mlx_load_png(path);
	if (!texture)
		return (system_invalid("Texture was null after mlx_load_png"));
	ctx->textures = ft_arraylist_add(ctx->textures, texture);
	if (!ctx->textures)
		return (system_invalid("List of textures was null after adding"));
	drawable->img = mlx_texture_to_image(mlx, texture);
	if (!drawable->img)
		return (system_invalid("Failed to create image from texture"));
	return (1);
}

int32_t	context_init_drawables_list(t_context *ctx)
{
	int32_t		i;
	t_arraylist	lst;

	i = 0;
	while (i <= EXIT)
	{
		lst = ft_arraylist_new(free);
		if (!lst)
			return (system_invalid("Failed to initialize list of drawables"));
		ctx->drawables = ft_arraylist_add(ctx->drawables, lst);
		if (!ctx->drawables)
			return (system_invalid("Failed to add list to ctx->drawables"));
		i++;
	}
	return (1);
}

int32_t	context_init(t_context *ctx, mlx_t *mlx)
{
	int32_t	i;
	int32_t	is_ok;

	i = 0;
	ctx->overflow_x_offset = 0;
	ctx->overflow_y_offset = 0;
	if (!context_init_drawables_list(ctx))
		return (system_invalid("Failed to init ctx drawables"));
	while (1)
	{
		if (i == HERO)
		{
			is_ok = context_load_asset(ctx, "./assets/hero.png", mlx, HERO);
			is_ok &= context_load_asset(ctx, "./assets/hero2.png", mlx, HERO);
		}
		else if (i == ITEM)
			is_ok = context_load_asset(ctx, "./assets/item.png", mlx, ITEM);
		else if (i == WALL)
			is_ok = context_load_asset(ctx, "./assets/wall.png", mlx, WALL);
		else if (i == EXIT)
			is_ok = context_load_asset(ctx, "./assets/exit.png", mlx, EXIT);
		else if (i > EXIT)
			break ;
		if (!is_ok)
			return (system_invalid("failed loading some asset"));
		i++;
	}
	return (1);
}

t_entity_type	entity_type_by_ch(char ch)
{
	t_entity_type	type;

	if (ch == 'P')
		type = HERO;
	else if (ch == 'C')
		type = ITEM;
	else if (ch == '1')
		type = WALL;
	else if (ch == 'E')
		type = EXIT;
	else
		type = FLOOR;
	return (type);
}

int	entity_drawables_init(
	t_entity *entity, t_arraylist drawables_ctx, t_game *game)
{
	t_drawable	*drawable_ctx;
	t_drawable	*drawable_copy;
	size_t		i;
	int			x;
	int			y;

	i = 0;
	while (i < ft_arraylist_len(drawables_ctx))
	{
		drawable_ctx = ft_arraylist_get(
				drawables_ctx, i);
		drawable_copy = malloc(sizeof(t_drawable));
		if (!drawable_copy)
			return (system_invalid("Failed allocing memory for drawable_copy"));
		entity->drawables = ft_arraylist_add(entity->drawables, drawable_copy);
		if (!entity->drawables)
			return (system_invalid("Entity->drawables list was null after add"));
		drawable_copy->img = drawable_ctx->img;
		x = entity->x * game->ctx.block_size + game->ctx.window_x_offset;
		y = entity->y * game->ctx.block_size + game->ctx.window_y_offset;
		drawable_copy->i = mlx_image_to_window(
				game->mlx, drawable_copy->img, x, y);
		if (drawable_copy->i < 0)
			return (system_invalid("Mlx_image_to_window failed"));
		drawable_copy->img->instances[drawable_copy->i].enabled = i == 0
			&& entity->type != EXIT;
		i++;
	}
	return (1);
}

t_direction	entity_hero_move(t_entity *entity, t_game *game)
{
	(void) entity;
	if (mlx_is_key_down(game->mlx, MLX_KEY_W))
		return (UP);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_D))
		return (RIGHT);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_S))
		return (DOWN);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_A))
		return (LEFT);
	else
		return (IDLE);
}

void	entity_hero_components_destroy(t_component *component)
{
	if (component->type == MOVEABLE)
		free(component->cmp);
	free(component);
}

int	entity_hero_components_init(t_entity *entity)
{
	t_component	*component;
	t_moveable	*moveable;

	entity->components = ft_arraylist_new(
			(t_vfun1)entity_hero_components_destroy);
	if (!entity->components)
		return (system_invalid("Failed to create components list for hero"));
	component = malloc(sizeof(t_component));
	if (!component)
		return (system_invalid("Failed to alloc component for hero"));
	component->type = MOVEABLE;
	moveable = malloc(sizeof(t_moveable));
	if (!moveable)
		return (system_invalid("Failed to alloc moveable for hero"));
	component->cmp = moveable;
	moveable->move = entity_hero_move;
	entity->components = ft_arraylist_add(entity->components, component);
	return (1);
}

int	entity_components_init(t_entity *entity, t_game *game)
{
	(void) game;
	if (entity->type == HERO)
		return (entity_hero_components_init(entity));
	else
		return (1);
}

t_entity	*entity_new(size_t y, size_t x, t_entity_type *type, t_game *game)
{
	t_entity		*entity;
	t_arraylist		drawables;

	entity = malloc(sizeof(t_entity));
	if (!entity)
		return (NULL);
	entity->type = *type;
	drawables = ft_arraylist_get(game->ctx.drawables, entity->type);
	entity->drawables = ft_arraylist_new(free);
	entity->x = x;
	entity->y = y;
	entity->components = NULL;
	if (!entity_drawables_init(entity, drawables, game))
		return (NULL);
	if (!entity_components_init(entity, game))
		return (NULL);
	if (entity->type == HERO)
		game->map.hero = entity;
	else if (entity->type == EXIT)
		game->map.exit = entity;
	else if (entity->type == ITEM)
		game->state.collectables_count++;
	return (entity);
}

void	entity_destroy(t_entity *entity)
{
	if (!entity || entity->type == EXIT)
		return ;
	if (entity->components)
		ft_arraylist_destroy(entity->components);
	if (entity->drawables)
		ft_arraylist_destroy(entity->drawables);
	free(entity);
}

void	entity_exit_destroy(t_entity *entity)
{
	if (!entity || entity->type != EXIT)
		return ;
	if (entity->components)
		ft_arraylist_destroy(entity->components);
	if (entity->drawables)
		ft_arraylist_destroy(entity->drawables);
	free(entity);
}

t_entity	*map_remove_floor(t_entity *entity)
{
	if (entity->type == FLOOR)
	{
		entity_destroy(entity);
		return (NULL);
	}
	else if (entity->type == EXIT)
		return (NULL);
	else
		return (entity);
}

int32_t	entities_init(t_game *game)
{
	if (ft_arraylist_transform2diarg(
			game->map.chart,
			(void *(*)(size_t, size_t, void*, void*)) entity_new,
		(void *) game,
		(t_vfun1) ft_nop) > 0)
		return (system_invalid("Failed to init entities"));
	ft_arraylist_transform2d(
		game->map.chart,
		(void *(*)(void*)) map_remove_floor,
		(t_vfun1) entity_destroy);
	return (1);
}

void	*map_transform_string_line(void *s)
{
	char			ch;
	t_arraylist		lst_type;
	size_t			i;
	size_t			len;
	t_entity_type	*type;

	lst_type = ft_arraylist_new(free);
	if (!lst_type)
		return (NULL);
	i = 0;
	len = ft_strlen(((char *)s));
	while (i < len)
	{
		ch = ((char *)s)[i];
		if (ch == '\n')
			break ;
		type = malloc(sizeof(t_entity_type));
		*type = entity_type_by_ch(ch);
		if (!type)
			return (NULL);
		lst_type = ft_arraylist_add(lst_type, type);
		if (!lst_type)
			return (NULL);
		i++;
	}
	return (lst_type);
}

size_t	map_height(t_map *map)
{
	return (ft_arraylist_len(map->chart));
}

size_t	map_width(t_map *map)
{
	return (ft_arraylist_len(
				ft_arraylist_get(map->chart, 0)));
}

int32_t	map_is_retangunlar(t_map *map)
{
	size_t		width;
	size_t		height;
	size_t		i;
	char 		*row;

	row = ft_arraylist_get(map->chart, 0);
	width = ft_strlen(row);
	height = map_height(map);
	i = 1;
	while(i < height)
	{
		row = ft_arraylist_get(map->chart, i);
		if (ft_strlen(row) != width)
			return (0);
		i++;
	}
	return (1);
}

int32_t	map_is_valid_first_row(t_map *map)
{
	char		*row;

	row = ft_arraylist_get(map->chart, 0);
	while(*row)
	{
		if (*row != '1')
			return (0);
		row++;
	}
	return (1);
}
int32_t	map_is_valid_last_row(t_map *map)
{
	size_t		height;
	char 		*row;

	height = map_height(map);
	row = ft_arraylist_get(map->chart, height - 1);
	while(*row)
	{
		if (*row != '1')
			return (0);
		row++;
	}
	return (1);
}

int32_t	map_is_valid_first_col(t_map *map)
{
	size_t		height;
	size_t		i;
	char 		*row;

	row = ft_arraylist_get(map->chart, 0);
	height = map_height(map);
	i = 0;
	while(i < height)
	{
		row = ft_arraylist_get(map->chart, i);
		if (*row != '1')
			return (0);
		i++;
	}
	return (1);
}

int32_t	map_is_valid_last_col(t_map *map)
{
	size_t		width;
	size_t		height;
	size_t		i;
	char 		*row;

	row = ft_arraylist_get(map->chart, 0);
	height = map_height(map);
	width = ft_strlen(row);
	i = 0;
	while(i < height)
	{
		row = ft_arraylist_get(map->chart, i);
		if (*(row + width - 1) != '1')
			return (0);
		i++;
	}
	return (1);
}

int32_t map_validate_entities_count(char *char_map)
{
	if (char_map['P'] != 1)
		return (system_invalid("Invalid count of P on map"));
	else if (char_map['E'] != 1)
		return (system_invalid("Invalid count of E on map"));
	else if (char_map['C'] < 1)
		return (system_invalid("Invalid count of C on map"));
	return (1);
}

int32_t	map_has_valid_entities(t_map *map)
{
	char 		char_map[256];
	size_t		width;
	size_t		height;
	size_t		i;
	char 		*row;

	row = ft_arraylist_get(map->chart, 0);
	width = ft_strlen(row);
	height = map_height(map);
	i = 1;
	ft_bzero(char_map, 256);
	while(i < height)
	{
		row = ft_arraylist_get(map->chart, i);
		while (*row)
		{
			if (!ft_strchr("10EPC", *row))
				return (system_invalid("Invalid character on map"));
			char_map[(int)*row]++;
			row++;
		}
		i++;
	}
	return (map_validate_entities_count(char_map));
}

int32_t	map_has_valid_path(t_map *map)
{
	(void) map;
	return (1);
}

int32_t map_is_valid(t_map *map)
{
	if (!map_is_retangunlar(map))
		return (system_invalid("Invalid map, should be retangular"));
	else if (!map_is_valid_first_row(map))
		return (system_invalid("Invalid first row for map"));
	else if (!map_is_valid_last_row(map))
		return (system_invalid("Invalid last row for map"));
	else if (!map_is_valid_first_col(map))
		return (system_invalid("Invalid first col for map"));
	else if (!map_is_valid_last_col(map))
		return (system_invalid("Invalid last col for map"));
	else if (!map_has_valid_entities(map))
		return (0);
	else if (!map_has_valid_path(map))
		return (system_invalid("Invalid map, no path to solve"));
	return (1);
}
int32_t	map_init(t_map *map, char *path)
{
	int		fd;
	char	*str;

	map->chart = ft_arraylist_new((free));
	if (!map->chart)
		return (system_invalid("Malloc fail\n"));
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (system_invalid("File open fail\n"));
	str = get_next_line(fd);
	while (str)
	{
		ft_chomp(str);
		map->chart = ft_arraylist_add(map->chart, str);
		if (!map->chart)
			return (system_invalid("Failed to add string to map->chart on init\n"));
		str = get_next_line(fd);
	}
	if (!map->chart)
		return (system_invalid("Failed to init map->chart"));
	if (!map_is_valid(map))
		return (0);
	if (ft_arraylist_transform(map->chart, map_transform_string_line,
			(t_vfun1) ft_arraylist_destroy) > 0)
		return (system_invalid("Failed to transform map"));
	return (1);
}

void	system_init_window_size(t_game *game)
{	
	game->ctx.block_size = 32;
	game->ctx.window_width = game->ctx.block_size * map_width(&game->map);
	game->ctx.window_height = game->ctx.block_size * map_height(&game->map);
	if (game->ctx.window_width < WIDTH)
	{
		game->ctx.window_x_offset = (WIDTH - game->ctx.window_width) / 2;
		game->ctx.window_width = WIDTH;
	}
	else
		game->ctx.window_x_offset = 0;
	if (game->ctx.window_height < HEIGHT)
	{
		game->ctx.window_y_offset = (HEIGHT - game->ctx.window_height) / 2;
		game->ctx.window_height = HEIGHT;
	}
	else
		game->ctx.window_y_offset = 0;
}

int32_t	system_init(t_game *game, char *map_path)
{
	ft_bzero(game, sizeof(t_game));
	game->ctx.drawables = ft_arraylist_new((t_vfun1) ft_arraylist_destroy);
	game->ctx.textures = ft_arraylist_new((t_vfun1) mlx_delete_texture);
	if (!game->ctx.drawables || !game->ctx.textures)
		return (
			system_panic(game, MEMORY_ERROR, "No memory to init drawables"));
	if (!map_init(&game->map, map_path))
		return (system_panic(game, ERROR, "Failed to init map"));
	system_init_window_size(game);
	game->mlx = mlx_init(game->ctx.window_width,
						 game->ctx.window_height,
						 "So Long", true);
	if (!game->mlx)
		return (system_panic(game, MLX_ERROR, NULL));
	if (!context_init(&game->ctx, game->mlx))
		return (system_panic(game, ERROR, "Failed to init assets"));
	if (!entities_init(game))
		return (system_panic(game, ERROR, "Failed to init entities"));
	mlx_loop_hook(game->mlx, (t_vfun1) system_loop, game);
	mlx_close_hook(game->mlx, system_quit_ok, game);
	game->state.gst = HERO_WAIT;
	return (OK);
}

int32_t map_is_valid_map_filename(char *filename)
{
	char	*p;

	p = filename;
	while (*p)
		p++;
	if (p == filename)
		return (system_invalid("Empty filename"));
	p -= 4;
	if (p < filename || ft_strncmp(p, ".ber", 4) != 0)
		return (system_invalid("Invalid extension for map file"));
	return (1);
}

int32_t	main(int args_len, char *args[])
{
	t_game	game;

	if (args_len != 2)
	{
		system_invalid("Invalid command. Usage example: so_long some_map.ber");
		return (1);
	}
	else if (!map_is_valid_map_filename(args[1]))
		return (1);
	if (system_init(&game, args[1]) == OK)
		mlx_loop(game.mlx);
	ft_arraylist_destroy(game.ctx.drawables);
	ft_arraylist_destroy(game.ctx.textures);
	ft_arraylist_destroy(game.map.chart);
	if (game.mlx)
		mlx_terminate(game.mlx);
	entity_exit_destroy(game.map.exit);
	return (game.exit_status);
}
