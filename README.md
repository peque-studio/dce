<h1 align=center>DragonCore Engine</h1>

<h2 align=center>Architecture</h2>

<h3 align=center>Assets</h3>

Loaded from compressed binary files in release mode
and from a json and a folder in debug mode.

<h3 align=center>Graphics</h3>

Vulkan. Silhouette and Toon shading support.

<h3 align=center>Audio</h3>

3D Audio support, likely.

<h3 align=center>World</h3>

A world is an asset with specification of which
entities to load. Also has the terrain

<h3 align=center>Gameplay</h3>

Gameplay code will be written in a scripting language.

<h3 align=center>Save System</h3>

All player data is saved to a compressed binary file.

<h2 align=center>Contributing</h2>

Write sensible code :)

<h3 align=center>Commit Message Format</h3>

The commit message should be in this format: `[MODULE:KIND] MESSAGE`
- `MODULE` can be DCm (math), DCg (graphics), DCd (debug), ... or git (git-related things), doc for docs, test for tests
- `KIND` can be either `fix` for general fixes, `add` for adding new stuff, `imp` for improvements (rename for example)
- `MESSAGE` is a short-ish message describing the changes. The explanation should be in the commit messages "body" if needed.

<h3 align=center>Issues/PRs</h3>

I can't always answer to issues and PR review takes time and I don't have much of it :( I for certain will answer in 0-1 days.

<h3 align=center>Naming</h3>

**Each symbol should have a prefix**:
- `DCg`/`dcg` for graphics
- `DCm`/`dcm` for maths
- `DCd`/`dcd` for debug
- Optional `i` suffix for internal names (for example `DCgi`)

**Functions**:
- Procedural (not `ImageGetAspectRatio`, but `GetImageAspectRatio`)
- Lower-case Prefix (<code><u>dcg</u>GetMaterialCache</code>)
- For creating objects: <code>preNew<u>ObjectType</u></code>
- For destroying objects: <code>preFree<u>ObjectType</u></code>
- For creating objects that are handled by their parent: <code>preGetNew<u>ObjectType</u></code>
- For returning lists: <code>size_t preGet<u>List</u>(..., <u>T</u> *list)</code>

**Types**:
- Do ***not*** use `_t`!
- Upper-case Prefix (<code><u>DCg</u>MaterialOptions</code>)
