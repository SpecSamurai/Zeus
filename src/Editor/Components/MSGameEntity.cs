namespace Zeus.Components;

public class MSGameEntity : MSEntity
{
    public MSGameEntity(List<GameEntity> entities) : base(entities)
    {
        Refresh();
    }
}
