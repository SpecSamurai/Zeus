namespace Frameworks;

public static class Functional
{
    public static TResult NavigateHierarchically<TInput, TResult>(
        TInput rootItem,
        Func<TInput, IReadOnlyCollection<TInput>> childrenProvider,
        Func<TInput, IReadOnlyCollection<TResult>, TResult> compositeMapper,
        Func<TInput, TResult> leafMapper,
        Func<TInput, bool>? recursionStopPredicate = null,
        Action? onRecursionEntry = null,
        Action? onRecursionExit = null)
    {
        onRecursionEntry?.Invoke();
        var children = childrenProvider(rootItem);

        if (recursionStopPredicate?.Invoke(rootItem) is not true && children.Any())
        {
            var mappedChildren = children
                .Select(child =>
                    NavigateHierarchically(
                        child,
                        childrenProvider,
                        compositeMapper,
                        leafMapper,
                        recursionStopPredicate,
                        onRecursionEntry,
                        onRecursionExit)
                )
                .ToList(); // Eager evaluation to trigger that recursion

            onRecursionExit?.Invoke();
            return compositeMapper(rootItem, mappedChildren);
        }

        onRecursionExit?.Invoke();
        return leafMapper(rootItem);
    }
}