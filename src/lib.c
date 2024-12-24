/* RandomArt
 * Functions exposed JS side
 */

#include "ast.h"
#include "lang.h"
#include "node.h"

#include "stb_image_write.h"

#define SIZE 512

static void *img;

void wfunc(void *context, void *data, int size) {
	img = data;
}

void *libRunScript(const char *SCRIPT) {
	Node *ast = langCompile(SCRIPT);
	byte *image = astDoFrame(ast, SIZE, SIZE, 1);

	stbi_write_png_to_func(wfunc, NULL, SIZE, SIZE, 3, image, SIZE * 3);

	return img;
}
