
/**
 * Function.prototype.bind is part of the ECMAScript 5 standard and is available
 * natively in newer browsers. For older browsers, we provide a backport.
 * This implementation is based on
 * http://webreflection.blogspot.com/2010/02/functionprototypebind.html
 *
 * See http://drupal.org/node/1025626
 */
if (Function.prototype.bind == null) {
  Function.prototype.bind = (function (slice) {
    function bind (context) {
      var self = this;
      if (1 < arguments.length) {
        // Extra arguments to send by default.
        var $arguments = slice.call(arguments, 1);
        return function () {
          return self.apply(
            context,
            arguments.length ?
              // If we received more than one argument, send them.
              $arguments.concat(slice.call(arguments)) :
              // If there's only one argument, no slice is needed.
              $arguments
          );
        };
      }
      // The simplest and most common case is one argument, so we optimize
      // for that case.
      return function () {
        // Even more optimization for when the function is called
        // without arguments.
        return arguments.length ? self.apply(context, arguments) : self.call(context);
        };
      }

      // Return the named function.
      return bind;

  }(Array.prototype.slice));
}
